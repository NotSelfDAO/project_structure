#pragma once
#ifndef AUTO_REGISTRAR_H
#define AUTO_REGISTRAR_H

#include "HierarchicalRegistry.h"

namespace backend_core
{
    namespace v1
    {
        struct AutoRouteRegistration
        {
            std::string serviceName;
            std::string moduleName;
            std::string funcName;
            HierarchicalRegistry::InternalHandleFn handler;
        };
        
        class AutoRegistryStore
        {
        public:
            static AutoRegistryStore& Instance();
            
            void Add(AutoRouteRegistration registration);

            std::vector<AutoRouteRegistration> Snapshot() const;

        private:
            mutable std::mutex mutex_;
            std::vector<AutoRouteRegistration> registrations_;
        };

        class AutoRegistrar
        {
        public:
            AutoRegistrar(
                const std::string& serviceName,
                const std::string& moduleName, 
                const std::string& funcName, 
                HierarchicalRegistry::InternalHandleFn handler);
            
        };

        class RegistryBootstrap
        {
        public:
            static void RegisterAll(
                HierarchicalRegistry& registry, 
                SymbolTable& symbols);
        };
    }
}

#define AUTO_REGISTER(service_name, module_name, func_name, handler_fn) \
    static ::backend_core::v1::AutoRegistrar auto_registrar_##handler_fn(service_name, module_name, func_name, handler_fn);

#endif // AUTO_REGISTRAR_H