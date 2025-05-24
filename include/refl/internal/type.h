#pragma once

#include "ref.h"

AMAZING_NAMESPACE_BEGIN

struct Parameter
{
    String name;
    String type;
};


class IType
{
public:
    virtual ~IType() = default;
    NODISCARD virtual String name() const = 0;
};

class IFunction : public IType
{
public:
    NODISCARD virtual uint32_t parameter_count() const = 0;
    NODISCARD virtual Parameter return_type() const = 0;
    NODISCARD virtual Parameter parameter_type(uint32_t index) const = 0;

    template <typename... Args>
    Any operator()(Args&&... args)
    {
        const Vector<Any> arguments = { std::forward<Args>(args)... };
        return invoke(arguments);
    }

    virtual Any invoke(const Vector<Any>& args) = 0;
};

class IMethod : public IType
{
public:
    NODISCARD virtual uint32_t parameter_count() const = 0;
    NODISCARD virtual Parameter return_type() const = 0;
    NODISCARD virtual Parameter parameter_type(uint32_t index) const = 0;

    template <typename... Args>
    Any operator()(const Ref& ref, Args&&... args)
    {
        const Vector<Any> arguments = { std::forward<Args>(args)... };
        return invoke(ref, arguments);
    }

    virtual Any invoke(const Ref& ref, const Vector<Any>& args) = 0;
};

class IClass : public IType
{
public:
    NODISCARD virtual uint32_t field_count() const = 0;
    NODISCARD virtual Parameter field_type(uint32_t index) const = 0;
    NODISCARD virtual Ref field(const Ref& obj, uint32_t index) const = 0;

    NODISCARD virtual uint32_t static_field_count() const = 0;
    NODISCARD virtual Parameter static_field_type(uint32_t index) const = 0;
    NODISCARD virtual Ref static_field(const Ref& obj, uint32_t index) const = 0;

    NODISCARD virtual uint32_t method_count() const = 0;
    NODISCARD virtual IMethod* method(uint32_t index) const = 0;

    NODISCARD virtual uint32_t static_method_count() const = 0;
    NODISCARD virtual IMethod* static_method(uint32_t index) const = 0;
};

AMAZING_NAMESPACE_END