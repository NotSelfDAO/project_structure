#pragma once
#ifndef ROUTE_KEY_H
#define ROUTE_KEY_H

#include <string>

namespace backend_core
{
    namespace v1
    {
        struct RouteKey
        {
            // this service is composed of multiple modules, and we want to identify which module the task belongs to. 
            // if this project is a single module project, then serviceName can be only used to identify the project name.
            // if this project is a multi-module project, then serviceName can be used to distinguish different services
            std::string serviceName;
            // this module is composed of multiple funcs, and we want to identify which func the task belongs to.
            std::string moduleName;
            // this func is a high-level func, which may call multiple low-level funcs. So we use funcName to identify the high-level func, instead of the low-level func. 
            std::string funcName;
            
            // the route is in the format of "serviceName.moduleName.funcName", for example, "UserService.UserModule.CreateUser"
            static RouteKey fromString(const std::string& route);

            std::string toString() const;

            std::string& getServiceName();
            std::string& getModuleName();
            std::string& getFuncName();

            bool operator==(const RouteKey& other) const;

        };
    }
}
#endif // ROUTE_KEY_H