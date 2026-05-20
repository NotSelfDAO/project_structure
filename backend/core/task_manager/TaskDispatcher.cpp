#include "TaskDispatcher.h"

namespace backend_core
{
    namespace v1
    {
        void TaskDispatcher::Register(const RouteKey& key, HandlerFn handler)
        {
            RegisterModuleFunc(
            key.serviceName,
            key.moduleName,
            key.funcName,
            std::move(handler));
        }
        void TaskDispatcher::RegisterModuleFunc(
            const std::string& serviceName,
            const std::string& moduleName,
            const std::string& funcName,
            HandlerFn handler)
        {
            moduleHandlers_[BuildModuleKey(serviceName, moduleName)][funcName] =
                std::move(handler);
        }

        TaskEnvelope TaskDispatcher::Dispatch(const TaskRequest& request) const
        {
            const auto moduleKey =
                BuildModuleKey(request.route.serviceName, request.route.moduleName);
            const auto moduleIt = moduleHandlers_.find(moduleKey);

            if (moduleIt == moduleHandlers_.end())
            {
                TaskEnvelope e;
                e.taskId = request.taskId;
                e.route = request.route;
                e.success = false;
                e.message = "module_not_found";
                return e;
            }

            const auto funcIt = moduleIt->second.find(request.route.funcName);
            if (funcIt == moduleIt->second.end())
            {
                TaskEnvelope e;
                e.taskId = request.taskId;
                e.route = request.route;
                e.success = false;
                e.message = "func_not_found";
                return e;
            }

            return funcIt->second(request);
        }

        std::string TaskDispatcher::BuildModuleKey(
            const std::string& serviceName,
            const std::string& moduleName) const
        {
            return serviceName + "/" + moduleName;
        }
    }
}