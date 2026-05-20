#pragma once
#ifndef ROUTE_SYMBOL_H
#define ROUTE_SYMBOL_H

#include <cstdint>

namespace backend_core
{
    namespace v1
    {
        struct RouteSymbol
        {
            std::uint32_t serviceId = 0;
            std::uint32_t moduleId = 0;
            std::uint32_t funcId = 0;

            bool operator==(const RouteSymbol& other) const
            {
                return serviceId == other.serviceId && moduleId == other.moduleId && funcId == other.funcId;
            }
        };
    }
}
#endif // ROUTE_SYMBOL_H