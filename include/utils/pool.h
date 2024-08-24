#pragma once
#include "containers/queue.h"
#include "containers/vector.h"
#include "traits/func.h"
#include <functional>

namespace Amazing
{
    class ITask
    {
    public:
        virtual ~ITask() = default;
        virtual void execute() {};
    };

    template<typename F, typename... Args>
        requires(Trait::is_invocable_r_v<void, F, Args...>)
    class Task : public ITask
    {
    public:
        Task(
#ifdef DEBUG
            std::string&& name,
#endif // DEBUG
            F&& f, Args&&... args) : 
#ifdef DEBUG
            m_name(std::forward<std::string>(name)),
#endif // DEBUG
            m_task(std::forward<F>(f)), m_args(std::forward<Args>(args)...) {}

        void execute() override
        {
            std::apply(m_task, m_args);
        }

    private:
        std::function<std::remove_pointer_t<F>> m_task;
        std::tuple<Args...> m_args;
#ifdef DEBUG
        std::string m_name;
#endif // DEBUG
    };

    namespace Pool
    {
        class TaskPool
        {
        public:
            TaskPool() = default;

            template<typename F, typename... Args>
            void add_task(
#ifdef DEBUG
                std::string&& name,
#endif // DEBUG
                F&& f, Args&&... args)
            {
                m_tasks.push(std::shared_ptr<ITask>(std::make_shared<Task<std::decay_t<F>, Args...>>(
#ifdef DEBUG
                    std::forward<std::string>(name),
#endif // DEBUG
                    std::forward<F>(f), std::forward<Args>(args)...)));
            }

            template<typename F, typename... Args>
            void add_task(Task<F, Args...>&& task)
            {
                m_tasks.push(std::shared_ptr<ITask>(std::make_shared<Task<F, Args...>>(std::forward<Task<F, Args...>>(task))));
            }

            std::shared_ptr<ITask> get_task()
            {
                std::shared_ptr<ITask> task = nullptr;
                m_tasks.pop(task);
                return task;
            }

        private:
            Container::ThreadSafeQueue<std::shared_ptr<ITask>> m_tasks;
        };


        constexpr static uint32_t DEFAULT_THREAD_POOL_SIZE = 4;

        class ThreadPool
        {
        public:
            ThreadPool(uint32_t size = DEFAULT_THREAD_POOL_SIZE);

            

        private:
            Container::ThreadSafeVector<std::thread> m_threads;
        };
    }
}