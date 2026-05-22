#pragma once
#ifndef HIERARCHICAL_REGISTRY_H
#define HIERARCHICAL_REGISTRY_H

#include <cstdint>
#include <unordered_map>
#include <functional>

#include "../task_manager/TaskModel.h"
#include "../route/SymbolTable.h"

namespace backend_core
{
    namespace v1
    {
        /**
         * @brief HierarchicalRegistry is responsible for managing the registration and dispatching of tasks based on a hierarchical route structure (service/module/function). It allows for efficient lookup and execution of task handlers based on the incoming task requests. The registry supports dynamic registration of handlers and provides a mechanism to handle task dispatching with proper error handling for unregistered routes.
         * 
         */
        class HierarchicalRegistry
        {
        public:
            using InternalHandleFn = std::function<backend_core::v1::TaskEnvelope(const backend_core::v1::TaskRequest&)>;

            /**
             * @brief Registers a task handler for a specific route.
             * 
             * @param route The route symbol representing the hierarchical path (service/module/function) for the task.
             * @param handler The function to handle tasks for the specified route.
             */
            void Register(const RouteSymbol& route, InternalHandleFn handler);

            /**
             * @brief Registers a task handler for a specific route using service, module, and function names.
             * 
             * @param symbols The symbol table used to resolve the route components.
             * @param serviceName The name of the service.
             * @param moduleName The name of the module.
             * @param funcName The name of the function.
             * @param handler The function to handle tasks for the specified route.
             */
            void RegisterByName(SymbolTable& symbols, const std::string& serviceName, const std::string& moduleName, const std::string& funcName, InternalHandleFn handler);

            /**
             * @brief Dispatches a task request to the appropriate handler based on the route.
             * 
             * @param request The task request to be dispatched.
             * @param symbols The symbol table used to resolve the route components.
             * @return backend_core::v1::TaskEnvelope The result of the task execution.
             */
            backend_core::v1::TaskEnvelope Dispatch(const backend_core::v1::TaskRequest& request, const SymbolTable& symbols) const;
        private:
            struct ModuleNode
            {
                std::unordered_map<std::uint32_t, InternalHandleFn> funcHandlers;
            };

            struct ServiceNode
            {
                std::unordered_map<std::uint32_t, ModuleNode> moduleNodes;
            };

            /**
             * @brief Generates a failure task envelope for a given request with a specified error message.
             * 
             * @param request The task request that failed.
             * @param message The error message describing the failure.
             * @return backend_core::v1::TaskEnvelope The failure task envelope.
             */
            static backend_core::v1::TaskEnvelope Fail(const backend_core::v1::TaskRequest& request, const std::string& message);

            std::unordered_map<std::uint32_t, ServiceNode> serviceNodes_;
        };
    }
}

#endif // HIERARCHICAL_REGISTRY_H