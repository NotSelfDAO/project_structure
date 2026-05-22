#pragma once
#ifndef BACKEND_TASK_POOL_H
#define BACKEND_TASK_POOL_H

#include <condition_variable>
#include <deque>
#include <mutex>

#include "BackendTask.h"

namespace backend_core
{
    namespace v1
    {
        /**
         * @brief BackendTaskPool manages a pool of backend tasks. It provides thread-safe methods to start, stop, and wait for tasks, as well as to push and pop tasks from the queue.
         */
        class BackendTaskPool
        {
        public:
            /**
             * @brief Starts the backend task pool, allowing tasks to be processed.
             * 
             */
            void Start();

            /**
             * @brief Stops the backend task pool, preventing further task processing.
             * 
             */
            void Stop();

            /**
             * @brief Waits for a task to be available and pushes it into the task queue.
             * 
             * @param task The backend task to be added to the queue.
             * @return true if the task was successfully added, false if the pool is stopped.
             */
            bool WaitAndPushTask(const BackendTask& task);
            
            /**
             * @brief Waits for a task to be available and pops it from the task queue.
             * 
             * @param task The backend task to be removed from the queue.
             * @return true if a task was successfully removed, false if the pool is stopped.
             */
            bool WaitAndPopTask(BackendTask& task);
        private:
            std::mutex mutex_;
            std::condition_variable cv_;
            std::deque<BackendTask> taskQueue_;
            bool stop_ = false;
        };
    }
}

#endif // BACKEND_TASK_POOL_H