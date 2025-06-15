//
// Created by AmazingBuff on 25-5-22.
//

#ifndef REFL_H
#define REFL_H

#include "internal/type.h"

namespace Amazing::Reflect
{
    namespace Internal
    {
        template <typename T>
        concept is_class = std::is_class_v<T>;

        template <typename T>
        concept is_method = std::is_member_function_pointer_v<T>;

        template <typename T>
        concept is_function = std::is_function_v<T> && !std::is_member_function_pointer_v<T>;

        template <typename T>
        concept is_fundamental = std::is_fundamental_v<T>;

        template <typename T>
        struct Select;

        template <is_class T>
        struct Select<T>
        {
            using type = NullClass;
        };

        template <is_method T>
        struct Select<T>
        {
            using type = NullMethod;
        };

        template <is_function T>
        struct Select<T>
        {
            using type = NullFunction;
        };

        template <is_fundamental T>
        struct Select<T>
        {
            using type = NullType;
        };

        template <typename T>
        using NullMetaInfo = typename Select<T>::type;
    }

    template <typename T>
    class MetaInfo : public Internal::NullMetaInfo<T> {};

#ifdef REFL_META
    #include "refl/refl.meta"
#endif
}

#endif //REFL_H
