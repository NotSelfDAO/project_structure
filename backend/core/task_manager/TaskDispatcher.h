#pragma once
#ifndef TASK_DISPATCHER_H
#define TASK_DISPATCHER_H

#include <functional>
#include <string>
#include <unordered_map>

#include "TaskModel.h"

namespace backend_core
{
    namespace v1
    {
    class TaskDispatcher
    {
    public:
        using HandlerFn = std::function<TaskEnvelope(const TaskRequest&)>;

        void Register(const RouteKey& key, HandlerFn handler);

        void RegisterModuleFunc(
            const std::string& serviceName,
            const std::string& moduleName,
            const std::string& funcName,
            HandlerFn handler);

        TaskEnvelope Dispatch(const TaskRequest& request) const;

        private:
            using FuncHandlers = std::unordered_map<std::string, HandlerFn>;
            std::unordered_map<std::string, FuncHandlers> moduleHandlers_;

            std::string BuildModuleKey(
                const std::string& serviceName,
                const std::string& moduleName) const;
        };
    }
}

#endif  // TASK_DISPATCHER_H