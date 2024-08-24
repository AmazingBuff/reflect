#pragma once

#include <queue>
#include <mutex>

namespace Amazing
{
	namespace Container
	{
        template<typename T>
        class ThreadSafeQueue
        {
        public:
            void push(T&& value)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_queue.push(std::forward<T>(value));
                m_cv.notify_one();
            }

            void pop(T& value)
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this]() { return !m_queue.empty(); });
                value = std::move(m_queue.front());
                m_queue.pop();
            }

            void try_pop(T& value)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_queue.empty())
                    return;
                value = std::move(m_queue.front());
                m_queue.pop();
            }

            void wait_pop(T& value, const std::chrono::microseconds& duration = 10)
            {
				std::unique_lock<std::mutex> lock(m_mutex);
                if (m_cv.wait_for(lock, duration, [this]() { return !m_queue.empty(); }) == std::cv_status::timeout)
                    return;
				value = std::move(m_queue.front());
				m_queue.pop();
			}

        private:
            std::queue<T> m_queue;
            std::mutex m_mutex;
            std::condition_variable m_cv;
        };
	}
}