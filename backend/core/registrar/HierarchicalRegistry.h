#pragma once
#include <cstdint>
#include <unordered_map>
#ifndef HIERARCHICAL_REGISTRY_H
#define HIERARCHICAL_REGISTRY_H

#include <functional>

#include "../task_manager/TaskModel.h"
#include "../route/SymbolTable.h"

namespace backend_core
{
    namespace v1
    {
        class HierarchicalRegistry
        {
        public:
            using InternalHandleFn = std::function<backend_core::v1::TaskEnvelope(const backend_core::v1::TaskRequest&)>;

            void Register(const RouteSymbol& route, InternalHandleFn handler);

            void RegisterByName(SymbolTable& symbols, const std::string& serviceName, const std::string& moduleName, const std::string& funcName, InternalHandleFn handler);

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

            static backend_core::v1::TaskEnvelope Fail(const backend_core::v1::TaskRequest& request, const std::string& message);

            std::unordered_map<std::uint32_t, ServiceNode> serviceNodes_;
        };
    }
}

#endif // HIERARCHICAL_REGISTRY_H