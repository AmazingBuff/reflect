//
// Created by AmazingBuff on 25-6-15.
//

#ifndef REFL_ACTION_H
#define REFL_ACTION_H

#include <clang/Frontend/FrontendAction.h>
#include <clang/AST/RecursiveASTVisitor.h>

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
        explicit ReflAttributeConsumer(llvm::StringRef InFile) : m_source_file(InFile) {}
        void HandleTranslationUnit(clang::ASTContext& Context) override;
    private:
        void WriteMetaInfoToFile() const;
    private:
        ReflAttributeVisitor m_visitor;
        std::string m_source_file;
        std::string m_output_directory = R"(E:/code/VS/reflect/meta)";
    };

    class ReflAttributeAction final : public clang::ASTFrontendAction
    {
    public:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile) override;
    };
}

#endif //REFL_ACTION_H
