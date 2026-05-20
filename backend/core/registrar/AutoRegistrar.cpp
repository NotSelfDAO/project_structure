#include "AutoRegistrar.h"
#include <mutex>

namespace backend_core
{
    namespace v1
    {
        AutoRegistryStore& AutoRegistryStore::Instance()
        {
            static AutoRegistryStore instance;
            return instance;
        }
            
        void AutoRegistryStore::Add(AutoRouteRegistration registration)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            registrations_.push_back(std::move(registration));
        }

        std::vector<AutoRouteRegistration> AutoRegistryStore::Snapshot() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return registrations_;
        }

        AutoRegistrar::AutoRegistrar(
            const std::string& serviceName, 
            const std::string& moduleName, 
            const std::string& funcName, 
            HierarchicalRegistry::InternalHandleFn handler)
        {
            AutoRegistryStore::Instance().Add(AutoRouteRegistration{
                std::move(serviceName), 
                std::move(moduleName), 
                std::move(funcName),
                std::move(handler)});
        }
            
        void RegistryBootstrap::RegisterAll(
            HierarchicalRegistry& registry, 
            SymbolTable& symbols)
        {
            for (const auto& entry : AutoRegistryStore::Instance().Snapshot()) 
            {
                registry.RegisterByName(
                    symbols,
                    entry.serviceName,
                    entry.moduleName,
                    entry.funcName,
                    entry.handler);
            }
        }

    }
}