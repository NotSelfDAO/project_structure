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
        class SymbolTable
        {
        public:
            std::uint32_t  InternService(const std::string& serviceName);
            
            std::uint32_t  InternModule(std::uint32_t serviceId, const std::string& moduleName );
            
            std::uint32_t  InternFunc(const std::uint32_t moduleId, const std::string& funcName);


            RouteSymbol  InternRoute(const std::string& serviceName, const std::string& moduleName, const std::string& funcName);
            
            std::optional<std::uint32_t> FindServiceId(const std::string&serviceName) const;

            std::optional<std::uint32_t> FindModuleId(std::uint32_t serviceId, const std::string& moduleName) const;

            std::optional<std::uint32_t> FindFuncId(std::uint32_t moduleId, const std::string& funcName) const;

        private:
            static std::string BuildScopedKey(std::uint32_t parentId, const std::string& childName);

            std::uint32_t InternServiceLocked(const std::string& serviceName);

            std::uint32_t InternModuleLocked(std::uint32_t serviceId, const std::string& moduleName);

            std::uint32_t InternFuncLocked(std::uint32_t moduleId, const std::string& funcName);

            // TODO: mutable什么意思
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
