//
// Created by AmazingBuff on 25-6-15.
//

#ifndef REFL_ACTION_H
#define REFL_ACTION_H

#include <clang/Frontend/FrontendAction.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/Tooling.h>

namespace Amazing::Reflect
{
    class ReflAttributeVisitor final : public clang::RecursiveASTVisitor<ReflAttributeVisitor>
    {
    public:
        bool VisitCXXRecordDecl(clang::CXXRecordDecl* RecordDecl);
        bool VisitEnumDecl(clang::EnumDecl* EnumDecl);
        [[nodiscard]] std::string GetMetaInfo() const;
    private:
        std::string m_meta_infos;
    };

    class ReflAttributeConsumer final : public clang::ASTConsumer
    {
    public:
        ReflAttributeConsumer(llvm::StringRef in_file, const std::vector<std::string>& input_directories, const std::string& output_directory);
        void HandleTranslationUnit(clang::ASTContext& Context) override;
    private:
        void WriteMetaInfoToFile() const;
    private:
        ReflAttributeVisitor m_visitor;
        std::string m_source_file;
        std::string m_output_directory;
        std::vector<std::string> m_input_directories;
    };

    class ReflAttributeAction final : public clang::ASTFrontendAction
    {
    public:
        ReflAttributeAction(const std::vector<std::string>& input_directories, const std::string& output_directory);
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile) override;
    private:
        std::vector<std::string> m_input_directories;
        std::string m_output_directory;
    };

    class ReflAttributeFactory final : public clang::tooling::FrontendActionFactory
    {
    public:
        ReflAttributeFactory(const std::vector<std::string>& input_directories, const std::string& output_directory);
        std::unique_ptr<clang::FrontendAction> create() override;
    private:
        std::vector<std::string> m_input_directories;
        std::string m_output_directory;
    };


}

#endif //REFL_ACTION_H
