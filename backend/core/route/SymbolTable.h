#pragma once
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <cstdint>
#include <string>
#include <optional>
#include <mutex>
#include <unordered_map>

#include "RouteSymbol.h"

namespace backend_core
{
    namespace v1
    {
        /**
         * @brief A symbol table for managing service, module, and function identifiers.
         * 
         */
        class SymbolTable
        {
        public:
            /**
             * @brief Interns a service name and returns its unique identifier.
             * 
             * @param serviceName The name of the service to intern.
             * @return std::uint32_t The unique identifier for the interned service.
             */
            std::uint32_t  InternService(const std::string& serviceName);
            
            /**
             * @brief Interns a module name under a specific service and returns its unique identifier.
             * 
             * @param serviceId The identifier of the service under which the module is to be interned.
             * @param moduleName The name of the module to intern.
             * @return std::uint32_t The unique identifier for the interned module.
             */
            std::uint32_t  InternModule(std::uint32_t serviceId, const std::string& moduleName );
            
            /**
             * @brief Interns a function name under a specific module and returns its unique identifier.
             * 
             * @param moduleId The identifier of the module under which the function is to be interned.
             * @param funcName The name of the function to intern.
             * @return std::uint32_t The unique identifier for the interned function.
             */
            std::uint32_t  InternFunc(const std::uint32_t moduleId, const std::string& funcName);

            /**
             * @brief Interns a route by interning its service, module, and function names and returns a RouteSymbol.
             * 
             * @param serviceName The name of the service to intern.
             * @param moduleName The name of the module to intern.
             * @param funcName The name of the function to intern.
             * @return RouteSymbol The RouteSymbol representing the interned route.
             */
            RouteSymbol  InternRoute(const std::string& serviceName, const std::string& moduleName, const std::string& funcName);
            
            /**
             * @brief Finds the unique identifier for a service by its name.
             * 
             * @param serviceName The name of the service to find.
             * @return std::optional<std::uint32_t> The unique identifier for the service if found, std::nullopt otherwise.
             */
            std::optional<std::uint32_t> FindServiceId(const std::string&serviceName) const;

            /**
             * @brief Finds the unique identifier for a module by its parent service ID and module name.
             * 
             * @param serviceId The identifier of the parent service.
             * @param moduleName The name of the module to find.
             * @return std::optional<std::uint32_t> The unique identifier for the module if found, std::nullopt otherwise.
             */
            std::optional<std::uint32_t> FindModuleId(std::uint32_t serviceId, const std::string& moduleName) const;

            /**
             * @brief Finds the unique identifier for a function by its parent module ID and function name.
             * 
             * @param moduleId The identifier of the parent module.
             * @param funcName The name of the function to find.
             * @return std::optional<std::uint32_t> The unique identifier for the function if found, std::nullopt otherwise.
             */
            std::optional<std::uint32_t> FindFuncId(std::uint32_t moduleId, const std::string& funcName) const;

        private:
            
            /**
             * @brief Builds a scoped key by combining a parent ID and a child name.
             * 
             * @param parentId The identifier of the parent.
             * @param childName The name of the child.
             * @return std::string The scoped key.
             */
            static std::string BuildScopedKey(std::uint32_t parentId, const std::string& childName);

            /**
             * @brief Internal method to intern a service name. Assumes the caller holds the mutex lock.
             * 
             * @param serviceName The name of the service to intern.
             * @return std::uint32_t The unique identifier for the interned service.
             */
            std::uint32_t InternServiceLocked(const std::string& serviceName);

            /**
             * @brief Internal method to intern a module name. Assumes the caller holds the mutex lock.
             * 
             * @param serviceId The identifier of the parent service.
             * @param moduleName The name of the module to intern.
             * @return std::uint32_t The unique identifier for the interned module.
             */
            std::uint32_t InternModuleLocked(std::uint32_t serviceId, const std::string& moduleName);

            /**
             * @brief Internal method to intern a function name. Assumes the caller holds the mutex lock.
             * 
             * @param moduleId The identifier of the parent module.
             * @param funcName The name of the function to intern.
             * @return std::uint32_t The unique identifier for the interned function.
             */
            std::uint32_t InternFuncLocked(std::uint32_t moduleId, const std::string& funcName);

            /**
             * @brief Mutex to protect access to the symbol table.
             * 
             * The mutable keyword allows this member to be modified even in const member functions,
             * enabling thread-safe read operations.
             */
            mutable std::mutex mutex_;
            
            // Start from 1 to avoid using 0 as a valid ID
            std::uint32_t nextServiceId_ = 1; 
            std::uint32_t nextModuleId_ = 1;
            std::uint32_t nextFuncId_ = 1;

            std::unordered_map<std::string, std::uint32_t> serviceNameToId_;
            std::unordered_map<std::string, std::uint32_t> moduleScopedNameToId_;
            std::unordered_map<std::string, std::uint32_t> funcScopedNameToId_;
        };
    }
}

#endif // SYMBOL_TABLE_H
