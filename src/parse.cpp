#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <filesystem>
#include <fstream>

#include "refl/plugin/refl_attr.h"
#include "refl/plugin/refl_action.h"

static clang::ParsedAttrInfoRegistry::Add<Amazing::Reflect::ReflAttrInfo> Reflect("refl", "parse [[refl]] attribute as reflection signal");

int main(int argc, const char* argv[])
{
    static llvm::cl::OptionCategory reflect_category("Reflect Options");
    static llvm::cl::list<std::string> input_files(
        "i",
        llvm::cl::ZeroOrMore,
        llvm::cl::value_desc("filename"),
        llvm::cl::desc("input file"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::list<std::string> input_directories(
        "I",
        llvm::cl::ZeroOrMore,
        llvm::cl::value_desc("directory"),
        llvm::cl::desc("input directory"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::opt<std::string> output_file(
        "o",
        llvm::cl::Optional,
        llvm::cl::value_desc("filename"),
        llvm::cl::desc("output file"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::opt<std::string> output_directory(
        "O",
        llvm::cl::Required,
        llvm::cl::value_desc("directory"),
        llvm::cl::desc("output directory"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::list<std::string> include_directories(
        "include",
        llvm::cl::ZeroOrMore,
        llvm::cl::value_desc("directory"),
        llvm::cl::desc("include directories"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::list<std::string> pch_files(
        "pch",
        llvm::cl::ZeroOrMore,
        llvm::cl::value_desc("filename"),
        llvm::cl::desc("pch file"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::list<std::string> definitions(
        "D",
        llvm::cl::ZeroOrMore,
        llvm::cl::value_desc("definition"),
        llvm::cl::desc("user definition"),
        llvm::cl::cat(reflect_category));

    llvm::cl::HideUnrelatedOptions(reflect_category);
    llvm::cl::ParseCommandLineOptions(argc, argv);
    llvm::cl::PrintOptionValues();

    uint32_t file_count = 0;
    for (const std::string& input_directory : input_directories)
    {
        std::filesystem::path dir = input_directory;
        if (std::filesystem::is_directory(dir))
        {
            for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(dir))
            {
                if (entry.is_regular_file())
                    file_count++;
            }
        }
    }

    for (const std::string& input_file : input_files)
    {
        if (std::filesystem::is_regular_file(input_file))
            file_count++;
    }

    std::vector<std::string> files(file_count);
    uint32_t file_index = 0;
    for (const std::string& input_directory : input_directories)
    {
        std::filesystem::path dir = input_directory;
        if (std::filesystem::is_directory(dir))
        {
            for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(dir))
            {
                if (entry.is_regular_file())
                {
                    files[file_index] = entry.path().generic_string();
                    file_index++;
                }
            }
        }
    }

    for (const std::string& input_file : input_files)
    {
        if (std::filesystem::is_regular_file(input_file))
        {
            files[file_index] = input_file;
            file_index++;
        }
    }

    std::vector<std::string> compile_commands;
    for (const std::string& dir : include_directories)
        compile_commands.push_back("-I" + dir);
    for (const std::string& pch_file : pch_files)
        compile_commands.push_back("-include" + pch_file);
    for (const std::string& def : definitions)
        compile_commands.push_back("-D" + def);

    compile_commands.emplace_back("-xc++");
    compile_commands.emplace_back("-std=c++latest");

    std::unique_ptr<clang::tooling::FixedCompilationDatabase> compilations =
        std::make_unique<clang::tooling::FixedCompilationDatabase>(".", compile_commands);

    std::vector<std::string> in_directories(input_directories.size());
    for (uint32_t i = 0; i < input_directories.size(); i++)
        in_directories[i] = input_directories[i];

    std::filesystem::path out_directory = output_directory.getValue();
    std::filesystem::path merge_directory = out_directory / "reflect";

    clang::tooling::ClangTool tool(*compilations, files);
    tool.run(std::make_unique<Amazing::Reflect::ReflAttributeFactory>(in_directories, merge_directory.generic_string()).get());

    // merge
    std::filesystem::path out_dir = out_directory / "refl";
    std::filesystem::path out_file = out_dir / "refl.meta";
    if (!files.empty() && std::filesystem::exists(merge_directory))
    {
        if (!std::filesystem::exists(out_dir))
            std::filesystem::create_directories(out_dir);

        std::ofstream out(out_file, std::ios::ate);
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(merge_directory))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".meta")
            {
                std::ifstream in(entry.path());
                if (in)
                {
                    std::string meta_info((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                    if (!meta_info.empty())
                        out << meta_info;
                }
                in.close();
            }
        }
        out.close();

        std::filesystem::remove_all(merge_directory);
    }

    if (!output_file.empty())
        std::filesystem::copy_file(out_file, output_file.getValue(), std::filesystem::copy_options::overwrite_existing);

    return 0;
}