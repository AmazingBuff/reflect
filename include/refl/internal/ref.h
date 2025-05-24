#pragma once

#include "any.h"

AMAZING_NAMESPACE_BEGIN
class Ref
{
public:
    template <typename T>
    explicit Ref(T& value) : m_ptr(static_cast<std::decay_t<std::remove_reference_t<T>>*>(&value)), m_id(type_id<std::decay_t<std::remove_reference_t<T>>>()) {}

    template<typename T>
    T& get() const
    {
        if (m_id != type_id<T>())
            throw std::bad_cast();

        return *static_cast<T*>(m_ptr);
    }

    template<typename T>
    NODISCARD bool is() const
    {
        return m_id == type_id<T>();
    }

    Ref(const Ref&) = default;
    Ref(Ref&&) = default;
    Ref& operator=(const Ref&) = default;
    Ref& operator=(Ref&&) = default;
private:
    void* m_ptr;
    size_t m_id;
};
AMAZING_NAMESPACE_END