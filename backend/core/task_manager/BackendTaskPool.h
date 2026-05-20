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
        class BackendTaskPool
        {
        public:
            void Start();
            void Stop();
            bool WaitAndPushTask(const BackendTask& task);
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