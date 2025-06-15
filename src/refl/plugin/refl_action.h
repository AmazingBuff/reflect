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
        [[nodiscard]] std::string GetMetaInfo() const { return m_meta_infos; }
    private:
        std::string m_meta_infos;
    };

    class ReflAttributeConsumer final : public clang::ASTConsumer
    {
    public:
        ReflAttributeConsumer(llvm::StringRef in_file, llvm::StringRef output_directory);
        void HandleTranslationUnit(clang::ASTContext& Context) override;
    private:
        void WriteMetaInfoToFile() const;
    private:
        ReflAttributeVisitor m_visitor;
        std::string m_source_file;
        std::string m_output_directory;
    };

    class ReflAttributeAction final : public clang::ASTFrontendAction
    {
    public:
        explicit ReflAttributeAction(llvm::StringRef output_directory);
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile) override;
    private:
        llvm::StringRef m_output_directory;
    };

    class ReflAttributeFactory final : public clang::tooling::FrontendActionFactory
    {
    public:
        explicit ReflAttributeFactory(llvm::StringRef output_directory);
        std::unique_ptr<clang::FrontendAction> create() override;
    private:
        llvm::StringRef m_output_directory;
    };


}

#endif //REFL_ACTION_H
