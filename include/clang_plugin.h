#pragma once

#include <clang/Frontend/FrontendAction.h>
#include <clang/AST/RecursiveASTVisitor.h>

namespace Amazing
{
    class FindNamedClassVisitor : public clang::RecursiveASTVisitor<FindNamedClassVisitor>
    {
    public:
        explicit FindNamedClassVisitor(clang::ASTContext* context) : Context(context) {}

        bool VisitCXXRecordDecl(clang::CXXRecordDecl* Declaration)
        {
            if (Declaration->getQualifiedNameAsString() == "n::m::C")
            {
                // getFullLoc uses the ASTContext's SourceManager to resolve the source
                // location and break it up into its line and column parts.
                clang::FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
                if (FullLocation.isValid())
                    llvm::outs() << "Found declaration at "
                    << FullLocation.getSpellingLineNumber() << ":"
                    << FullLocation.getSpellingColumnNumber() << "\n";
            }
            return true;
        }
    private:
        clang::ASTContext* Context;
    };

    class FindNamedClassConsumer : public clang::ASTConsumer
    {
    public:
        explicit FindNamedClassConsumer(clang::ASTContext* context) : Visitor(context) {}

        void HandleTranslationUnit(clang::ASTContext& Context) override
        {
            // Traversing the translation unit decl via a RecursiveASTVisitor
            // will visit all nodes in the AST.
            Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        }
    private:
        // A RecursiveASTVisitor implementation.
        FindNamedClassVisitor Visitor;
    };

    class FindNamedClassAction : public clang::ASTFrontendAction
    {
    public:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile) override
        {
            return std::make_unique<FindNamedClassConsumer>(&Compiler.getASTContext());
        }
    };
}