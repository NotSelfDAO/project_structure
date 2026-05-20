#include "RouteKey.h"

#include <stdexcept>

namespace backend_core
{
    namespace v1
    {
        RouteKey RouteKey::fromString(const std::string& route)
        {
            // 只解析两个点
            RouteKey key;
            size_t firstDot = route.find('.');
            size_t secondDot = route.find('.', firstDot + 1);
            if (firstDot == std::string::npos || secondDot == std::string::npos)
            {
                // 之后统一使用一个异常类来处理这个错误，暂时先用std::invalid_argument
                throw std::invalid_argument("Invalid route format. Expected format: serviceName.moduleName.funcName");
            }
            key.serviceName = route.substr(0, firstDot);
            key.moduleName = route.substr(firstDot + 1, secondDot - firstDot - 1);
            key.funcName = route.substr(secondDot + 1);
            return key;
        }

        std::string RouteKey::toString() const
        {
            return serviceName + "." + moduleName + "." + funcName;
        }

        std::string& RouteKey::getServiceName()
        {
            return serviceName;
        }

        std::string& RouteKey::getModuleName()
        {
            return moduleName;
        }

        std::string& RouteKey::getFuncName()
        {
            return funcName;
        }

        bool RouteKey::operator==(const RouteKey& other) const
        {
            return serviceName == other.serviceName &&
                   moduleName == other.moduleName &&
                   funcName == other.funcName;
        }

    }
}