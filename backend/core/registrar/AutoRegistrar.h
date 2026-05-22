#pragma once
#ifndef AUTO_REGISTRAR_H
#define AUTO_REGISTRAR_H

#include "HierarchicalRegistry.h"

namespace backend_core
{
    namespace v1
    {
        /**
         * @brief Struct to hold auto-registration information for a route. This is used internally by the AutoRegistrar and AutoRegistryStore to manage route registrations that are defined using the AUTO_REGISTER macro.
         * 
         */
        struct AutoRouteRegistration
        {
            std::string serviceName;
            std::string moduleName;
            std::string funcName;
            HierarchicalRegistry::InternalHandleFn handler;
        };
        
        /**
         * @brief AutoRegistryStore manages the storage of auto-registered routes. It provides methods to add new registrations and retrieve a snapshot of all registrations. The store is thread-safe, allowing concurrent additions and retrievals of registrations.
         * 
         */
        class AutoRegistryStore
        {
        public:
            /**
             * @brief Get the singleton instance of the AutoRegistryStore.
             * 
             * @return AutoRegistryStore& The singleton instance of the store.
             */
            static AutoRegistryStore& Instance();
            
            /**
             * @brief Add a new auto-registration to the store. This method is thread-safe and can be called concurrently from multiple threads.
             * 
             * @param registration The AutoRouteRegistration to add to the store.
             */
            void Add(AutoRouteRegistration registration);

            /**
             * @brief Get a snapshot of all auto-registrations in the store. This method is thread-safe and can be called concurrently from multiple threads.
             * 
             * @return std::vector<AutoRouteRegistration> A vector containing all auto-registrations.
             */
            std::vector<AutoRouteRegistration> Snapshot() const;

        private:
            mutable std::mutex mutex_;
            std::vector<AutoRouteRegistration> registrations_;
        };

        /**
         * @brief AutoRegistrar is responsible for automatically registering routes with the AutoRegistryStore. It is used internally by the AUTO_REGISTER macro.
         */
        class AutoRegistrar
        {
        public:
            /**
             * @brief Construct a new AutoRegistrar object and add the provided route registration to the AutoRegistryStore.
             * 
             * @param serviceName The name of the service for the route.
             * @param moduleName The name of the module for the route.
             * @param funcName The name of the function for the route.
             * @param handler The handler function for the route, which will be called when the route is dispatched.
             */
            AutoRegistrar(
                const std::string& serviceName,
                const std::string& moduleName, 
                const std::string& funcName, 
                HierarchicalRegistry::InternalHandleFn handler);
            
        };

        /**
         * @brief RegistryBootstrap is responsible for registering all auto-registered routes with a given hierarchical registry and symbol table. It provides a static method to perform this registration, which should be called during application initialization to ensure that all routes are registered before they are used.
         */
        class RegistryBootstrap
        {
        public:
            /**
             * @brief Register all auto-registered routes with the provided hierarchical registry and symbol table.
             * 
             * @param registry The hierarchical registry to register routes with.
             * @param symbols The symbol table to register routes with.
             */
            static void RegisterAll(
                HierarchicalRegistry& registry, 
                SymbolTable& symbols);
        };
    }
}

#define AUTO_REGISTER(service_name, module_name, func_name, handler_fn) \
    static ::backend_core::v1::AutoRegistrar auto_registrar_##handler_fn(service_name, module_name, func_name, handler_fn);

#endif // AUTO_REGISTRAR_H