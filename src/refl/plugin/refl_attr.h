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
        ReflAttrInfo();
        bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
        AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;
    };
}
#endif //REFL_ATTR_H
