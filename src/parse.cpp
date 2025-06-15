#include <refl/refl.h>
#include "refl/plugin/refl_action.h"
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <filesystem>
#include <iostream>



int main()
{
    int argc = 5;
    const char* argv[] = {"-I", "input.cpp", "input.h", "-o", "output.meta", };

    llvm::cl::ResetCommandLineParser();


    static llvm::cl::OptionCategory Reflect_Category("Reflect Options");
    static llvm::cl::list<std::string> Input_File(
        "i",
        llvm::cl::value_desc("input file"),
        llvm::cl::cat(Reflect_Category));
    static llvm::cl::opt<std::string> Input_Directory(
        "I",
        llvm::cl::desc("input directory"),
        llvm::cl::cat(Reflect_Category));
    static llvm::cl::opt<std::string> Output_File(
        "o",
        llvm::cl::desc("output file"),
        llvm::cl::cat(Reflect_Category));
    static llvm::cl::opt<std::string> Output_Directory(
        "O",
        llvm::cl::desc("output directory"),
        llvm::cl::cat(Reflect_Category));

    llvm::cl::HideUnrelatedOptions(Reflect_Category);
    llvm::cl::ParseCommandLineOptions(argc, argv, "Tool description\n");


    // clang::Expected<clang::tooling::CommonOptionsParser> expected_parser = clang::tooling::CommonOptionsParser::create(argc, argv, Reflect_Category, llvm::cl::OneOrMore);
    //
    // if (!expected_parser) {
    //     llvm::errs() << expected_parser.takeError();
    //     return 1;
    // }
    // clang::tooling::CommonOptionsParser& parser = expected_parser.get();
    // const std::vector<std::string>& files = parser.getSourcePathList();



    //clang::tooling::RefactoringTool Tool(OptionsParser.getCompilations(), Files);

    if (!Input_File.empty())
    {
        //std::cout << Input_File << std::endl;
    }

    if (!Output_File.empty())
    {
        std::cout << Output_File << std::endl;
    }

    std::string ErrorMsg = "cont";

        //std::unique_ptr<clang::tooling::FixedCompilationDatabase> database =
        //    clang::tooling::FixedCompilationDatabase::loadFromCommandLine(argc, argv, ErrorMsg);

        //clang::tooling::ClangTool Tool(*database, {"sourceFiles"});
}