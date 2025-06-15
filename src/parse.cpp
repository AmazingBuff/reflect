#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <filesystem>

#include "refl/plugin/refl_attr.h"
#include "refl/plugin/refl_action.h"

static clang::ParsedAttrInfoRegistry::Add<Amazing::Reflect::ReflAttrInfo> Reflect("refl", "parse [[refl]] attribute as reflection signal");

int main(int Argc, const char* Argv[])
{
    int argc = 5;
    const char* argv[] = {
        "reflect.exe",
        "-i", "E:/code/VS/reflect/cmake-build-debug/input.cpp",
        "-O", "E:/code/VS/reflect/meta"
    };

    static llvm::cl::OptionCategory reflect_category("Reflect Options");
    static llvm::cl::list<std::string> input_files(
        "i",
        llvm::cl::value_desc("filename"),
        llvm::cl::desc("input file"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::list<std::string> input_directories(
        "I",
        llvm::cl::value_desc("directory"),
        llvm::cl::desc("input directory"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::opt<std::string> output_file(
        "o",
        llvm::cl::value_desc("filename"),
        llvm::cl::desc("output file"),
        llvm::cl::cat(reflect_category));
    static llvm::cl::opt<std::string> output_directory(
        "O",
        llvm::cl::Required,
        llvm::cl::value_desc("directory"),
        llvm::cl::desc("output directory"),
        llvm::cl::cat(reflect_category));

    llvm::cl::HideUnrelatedOptions(reflect_category);
    llvm::cl::ParseCommandLineOptions(argc, argv, "Tool description\n");
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


    std::unique_ptr<clang::tooling::FixedCompilationDatabase> compilations = std::make_unique<clang::tooling::FixedCompilationDatabase>(".", std::vector<std::string>());
    clang::tooling::ClangTool tool(*compilations, files);
    return tool.run(std::make_unique<Amazing::Reflect::ReflAttributeFactory>(output_directory).get());
}