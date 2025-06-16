//
// Created by AmazingBuff on 25-6-15.
//

#include "refl_attr.h"

namespace Amazing::Reflect
{
    ReflAttrInfo::ReflAttrInfo()
    {
        static constexpr Spelling spellings[] = {{clang::ParsedAttr::AS_GNU, "refl"}, {clang::ParsedAttr::AS_CXX11, "refl"}};
        Spellings = spellings;
        OptArgs = 0;
    }

    bool ReflAttrInfo::diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const
    {
        if (!isa<clang::CXXRecordDecl>(D) && !isa<clang::EnumDecl>(D))
        {
            S.Diag(Attr.getLoc(), clang::diag::warn_attribute_wrong_decl_type) << Attr << Attr.isRegularKeywordAttribute() << clang::ExpectedFunctionOrClassOrEnum;
            return false;
        }
        return true;
    }

    clang::ParsedAttrInfo::AttrHandling ReflAttrInfo::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const
    {
        // Check if the decl is at file scope.
        if (!D->getDeclContext()->isFileContext())
        {
            S.Diag(Attr.getLoc(), S.getDiagnostics().getCustomDiagID(clang::DiagnosticsEngine::Error, "'refl' attribute only allowed at file scope"));
            return AttributeNotApplied;
        }

        D->addAttr(clang::AnnotateAttr::Create(S.Context, "refl", nullptr, 0, Attr.getRange()));

        return AttributeApplied;
    }

}