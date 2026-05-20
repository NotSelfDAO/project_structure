#include "BackendTaskPool.h"
#include <mutex>

namespace backend_core
{
    namespace v1
    {
        void BackendTaskPool::Start()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = false;
        }

        void BackendTaskPool::Stop()
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                stop_ = true;
            }
            cv_.notify_all();
        }

        bool BackendTaskPool::WaitAndPushTask(const BackendTask& task)
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (stop_)
                {
                    return false;
                }
                taskQueue_.push_back(task);
            }
            cv_.notify_one();
            return true;
        }

        bool BackendTaskPool::WaitAndPopTask(BackendTask& task)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return stop_ || !taskQueue_.empty(); });
            if (stop_ && taskQueue_.empty())
            {
                return false;
            }
            task = taskQueue_.front();
            taskQueue_.pop_front();
            return true;
        }

    }
}