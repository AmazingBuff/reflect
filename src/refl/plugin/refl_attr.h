//
// Created by AmazingBuff on 25-6-15.
//

#ifndef REFL_ATTR_H
#define REFL_ATTR_H

#include <clang/Sema/Sema.h>

namespace Amazing::Reflect
{
    struct ReflAttrInfo final : clang::ParsedAttrInfo
    {
        ReflAttrInfo()
        {
            static constexpr Spelling spellings[] = {{clang::ParsedAttr::AS_GNU, "refl"}, {clang::ParsedAttr::AS_CXX11, "refl"}};
            Spellings = spellings;
            OptArgs = 0;
        }

        bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override
        {
            if (!isa<clang::CXXRecordDecl>(D))
            {
                S.Diag(Attr.getLoc(), clang::diag::warn_attribute_wrong_decl_type) << Attr << Attr.isRegularKeywordAttribute() << clang::ExpectedClass;
                return false;
            }
            return true;
        }

        AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override
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
    };
    static clang::ParsedAttrInfoRegistry::Add<ReflAttrInfo> Reflect("refl", "parse [[refl]] attribute as reflection signal");
}
#endif //REFL_ATTR_H
