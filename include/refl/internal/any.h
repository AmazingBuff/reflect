#pragma once

#include <type_traits>
#include <typeindex>
#include <astd/astd.h>

AMAZING_NAMESPACE_BEGIN

template <typename T>
size_t type_id()
{
    static const size_t id = std::type_index(typeid(T)).hash_code();
    return id;
}


class Any
{
    using Dtor = void(*)(void*);
public:
    // store a void type
    Any() : m_ptr(nullptr), m_dtor(nullptr), m_id(type_id<void>()) {}

    template<typename T>
    explicit Any(T&& value, Dtor&& dtor = nullptr) : m_ptr(new std::decay_t<T>(std::forward<T>(value))), m_id(type_id<std::decay_t<T>>())
    {
        if (dtor)
            m_dtor = dtor;
        else
            m_dtor = [](void* ptr) { delete static_cast<std::decay_t<T>*>(ptr); };
    }

    ~Any()
    {
        m_dtor(m_ptr);
        m_ptr = nullptr;
    }

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

    Any(const Any&) = delete;
    Any(Any&&) = delete;
    Any& operator=(const Any&) = delete;
    Any& operator=(Any&&) = delete;
private:
    void* m_ptr;
    Dtor m_dtor;
    size_t m_id;
};

AMAZING_NAMESPACE_END