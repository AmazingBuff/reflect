//
// Created by AmazingBuff on 25-5-22.
//

#ifndef RELF_H
#define RELF_H

#include "internal/type.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

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
    using type = IClass;
};

template <is_method T>
struct Select<T>
{
    using type = IMethod;
};

template <is_function T>
struct Select<T>
{
    using type = IFunction;
};

template <is_fundamental T>
struct Select<T>
{
    using type = IType;
};

template <typename T>
using IMetaInfo = typename Select<T>::type;

INTERNAL_NAMESPACE_END

template <typename T>
class MetaInfo : public Internal::IMetaInfo<T> {};

AMAZING_NAMESPACE_END

#endif //RELF_H
