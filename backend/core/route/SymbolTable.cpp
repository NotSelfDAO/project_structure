#include "SymbolTable.h"

namespace backend_core
{
    namespace v1
    {
        std::uint32_t  SymbolTable::InternService(const std::string& serviceName)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return InternServiceLocked(serviceName);
        }
    
        std::uint32_t  SymbolTable::InternModule(std::uint32_t serviceId, const std::string& moduleName )
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return InternModuleLocked(serviceId, moduleName);
        }
        
        std::uint32_t  SymbolTable::InternFunc(const std::uint32_t moduleId, const std::string& funcName)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return InternFuncLocked(moduleId, funcName);
        }


        RouteSymbol  SymbolTable::InternRoute(const std::string& serviceName, const std::string& moduleName, const std::string& funcName)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            const auto serviceId = InternServiceLocked(serviceName);
            const auto moduleId = InternModuleLocked(serviceId, moduleName);
            const auto funcId = InternFuncLocked(moduleId, funcName);
            return RouteSymbol{serviceId, moduleId, funcId};
        }
        
        std::optional<std::uint32_t> SymbolTable::FindServiceId(const std::string&serviceName) const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            const auto it = serviceNameToId_.find(serviceName);
            if (it == serviceNameToId_.end()) {
            return std::nullopt;
            }
            return it->second;
        }

        std::optional<std::uint32_t> SymbolTable::FindModuleId(std::uint32_t serviceId, const std::string& moduleName) const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            const auto it = moduleScopedNameToId_.find(BuildScopedKey(serviceId, moduleName));
            if (it == moduleScopedNameToId_.end()) {
            return std::nullopt;
            }
            return it->second;
        }

        std::optional<std::uint32_t> SymbolTable::FindFuncId(std::uint32_t moduleId, const std::string& funcName) const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            const auto it = funcScopedNameToId_.find(BuildScopedKey(moduleId, funcName));
            if (it == funcScopedNameToId_.end()) {
            return std::nullopt;
            }
            return it->second;
        }

        std::string SymbolTable::BuildScopedKey(std::uint32_t parentId, const std::string& childName)
        {
            return std::to_string(parentId) + "/" + childName;
        }

        std::uint32_t SymbolTable::InternServiceLocked(const std::string& serviceName)
        {
            const auto existed = serviceNameToId_.find(serviceName);
            if (existed != serviceNameToId_.end()) {
            return existed->second;
            }
            const auto id = nextServiceId_++;
            serviceNameToId_[serviceName] = id;
            return id;
        }

        std::uint32_t SymbolTable::InternModuleLocked(std::uint32_t serviceId, const std::string& moduleName)
        {
            const auto key = BuildScopedKey(serviceId, moduleName);
            const auto existed = moduleScopedNameToId_.find(key);
            if (existed != moduleScopedNameToId_.end()) {
            return existed->second;
            }
            const auto id = nextModuleId_++;
            moduleScopedNameToId_[key] = id;
            return id;
        }

        std::uint32_t SymbolTable::InternFuncLocked(std::uint32_t moduleId, const std::string& funcName)
        {
            const auto key = BuildScopedKey(moduleId, funcName);
            const auto existed = funcScopedNameToId_.find(key);
            if (existed != funcScopedNameToId_.end()) {
            return existed->second;
            }
            const auto id = nextFuncId_++;
            funcScopedNameToId_[key] = id;
            return id;
                }
    }
}