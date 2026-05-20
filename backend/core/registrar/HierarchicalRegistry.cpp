#include "HierarchicalRegistry.h"

namespace backend_core
{
    namespace v1
    {
        void HierarchicalRegistry::Register(const RouteSymbol& route, InternalHandleFn handler)
        {
            serviceNodes_[route.serviceId].moduleNodes[route.moduleId].funcHandlers[route.funcId] = std::move(handler);
        }

        void HierarchicalRegistry::RegisterByName(SymbolTable& symbols, const std::string& serviceName, const std::string& moduleName, const std::string& funcName, InternalHandleFn handler)
        {

            Register(symbols.InternRoute(serviceName, moduleName, funcName),std::move(handler));
        }

        backend_core::v1::TaskEnvelope HierarchicalRegistry::Dispatch(const backend_core::v1::TaskRequest& request, const SymbolTable& symbols) const
        {
            const auto serviceId = symbols.FindServiceId(request.route.serviceName);
            if (!serviceId.has_value()) 
            {
                return Fail(request, "service_not_found");
            }

            const auto moduleId =
                symbols.FindModuleId(*serviceId, request.route.moduleName);
            if (!moduleId.has_value()) 
            {
                return Fail(request, "module_not_found");
            }

            const auto funcId = symbols.FindFuncId(*moduleId, request.route.funcName);
            if (!funcId.has_value()) 
            {
                return Fail(request, "func_not_found");
            }

            const auto serviceIt = serviceNodes_.find(*serviceId);
            if (serviceIt == serviceNodes_.end()) 
            {
                return Fail(request, "service_not_registered");
            }
            const auto moduleIt = serviceIt->second.moduleNodes.find(*moduleId);
            if (moduleIt == serviceIt->second.moduleNodes.end()) 
            {
                return Fail(request, "module_not_registered");
            }
            const auto funcIt = moduleIt->second.funcHandlers.find(*funcId);
            if (funcIt == moduleIt->second.funcHandlers.end()) 
            {
                return Fail(request, "func_not_registered");
            }

            return funcIt->second(request);
        }

        backend_core::v1::TaskEnvelope HierarchicalRegistry::Fail(const backend_core::v1::TaskRequest& request, const std::string& message)
        {
            backend_core::v1::TaskEnvelope envelope;
            envelope.taskId = request.taskId;
            envelope.route = request.route;
            envelope.success = false;
            envelope.message = message;
            return envelope;
        }
    }
}