#pragma once

#include <vector>
#include <mutex>

namespace Amazing
{
	namespace Container
	{
		template<typename T>
		class ThreadSafeVector
		{
		public:
			void push_back(T&& value)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_vector.push_back(std::forward<T>(value));
				m_cv.notify_one();
			}

			void pop_back(T& value)
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_cv.wait(lock, [this]() { return !m_vector.empty(); });
				value = std::move(m_vector.back());
				m_vector.pop_back();
			}

			void try_pop_back(T& value)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if (m_vector.empty())
					return;
				value = std::move(m_vector.back());
				m_vector.pop_back();
			}

			void wait_pop_back(T& value, const std::chrono::microseconds& duration = 10)
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				if (m_cv.wait_for(lock, duration, [this]() { return !m_vector.empty(); }) == std::cv_status::timeout)
					return;
				value = std::move(m_vector.back());
				m_vector.pop_back();
			}

			const T& operator[](size_t index) const
			{
				return m_vector[index];
			}

		private:
			std::vector<T> m_vector;
			std::mutex m_mutex;
			std::condition_variable m_cv;
		};
	}
}