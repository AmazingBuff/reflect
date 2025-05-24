#pragma once

#include "list.h"
#include "astd/base/except.h"

AMAZING_NAMESPACE_BEGIN
template<typename Tp, template <typename> typename Alloc = Allocator>
class Queue
{
    using allocator = Alloc<Internal::ListNode<Tp>>;
public:
    Queue() : m_size(0)
    {
        m_head = allocator::allocate(1);
        m_head->val = Tp();
        m_head->prev = nullptr;
        m_head->next = nullptr;
    }

    ~Queue()
    {
        clear();

        allocator::deallocate(m_head);
        m_head = nullptr;
    }

    void enqueue(Tp&& value)
    {
        Internal::ListNode<Tp>* node = allocator::allocate(1);
        node->val = value;
        node->next = nullptr;

        if (!empty())
        {
            m_head->prev->next = node;
            node->prev = m_head->prev;
        }
        else
        {
            m_head->next = node;
            node->prev = m_head;
        }

        m_head->prev = node;
        m_size++;
    }

    void dequeue()
    {
        if (empty())
            return;

        Internal::ListNode<Tp>* node = m_head->next;

        if (m_size > 1)
            m_head->next = node->next;
        else
        {
            m_head->prev = nullptr;
            m_head->next = nullptr;
        }

        allocator::deallocate(node);
        node = nullptr;

        m_size--;
    }

    NODISCARD bool empty() const
    {
        return m_size == 0;
    }

    NODISCARD size_t size() const
    {
        return m_size;
    }

    NODISCARD Tp front() const
    {
        if (!empty())
            return m_head->next->val;

        throw AStdException(AStdError::NO_VALID_PARAMETER);
    }

    void clear()
    {
        Internal::ListNode<Tp>* node = m_head->next;
        while (node != nullptr)
        {
            Internal::ListNode<Tp>* next = node->next;

            allocator::deallocate(node);
            node = next;
        }
        m_head->next = nullptr;

        m_size = 0;
    }

private:
    Internal::ListNode<Tp>* m_head;
    size_t m_size;
};


AMAZING_NAMESPACE_END