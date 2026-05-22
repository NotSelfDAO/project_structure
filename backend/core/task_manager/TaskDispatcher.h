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
        /**
         * @brief TaskDispatcher is responsible for routing incoming task requests to the appropriate handler functions.
         * It maintains a registry of handlers organized by service, module, and function names.
         */
        class TaskDispatcher
        {
        public:
            using HandlerFn = std::function<TaskEnvelope(const TaskRequest&)>;

            /**
            * @brief Register a handler function for a specific route key.
            * 
            * @param key The route key identifying the service, module, and function.
            * @param handler The handler function to register.
            */
            void Register(const RouteKey& key, HandlerFn handler);

            /**
             * @brief Register a handler function for a specific service, module, and function.
             * 
             * @param serviceName The name of the service.
             * @param moduleName The name of the module.
             * @param funcName The name of the function.
             * @param handler The handler function to register.
             */
            void RegisterModuleFunc(
                const std::string& serviceName,
                const std::string& moduleName,
                const std::string& funcName,
                HandlerFn handler);

            /**
             * @brief Dispatch an incoming task request to the appropriate handler function based on the route.
             * 
             * @param request The incoming task request to dispatch.
             * @return TaskEnvelope The response envelope from the handler function.
             */
            TaskEnvelope Dispatch(const TaskRequest& request) const;

        private:
            using FuncHandlers = std::unordered_map<std::string, HandlerFn>;
            std::unordered_map<std::string, FuncHandlers> moduleHandlers_;

            /**
             * @brief Build a unique key for a module within a service.
             * 
             * @param serviceName The name of the service.
             * @param moduleName The name of the module.
             * @return std::string The unique key for the module.
             */
            std::string BuildModuleKey(
                const std::string& serviceName,
                const std::string& moduleName) const;
        };
    }
}

#endif  // TASK_DISPATCHER_H