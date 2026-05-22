#pragma once
#ifndef ROUTE_SYMBOL_H
#define ROUTE_SYMBOL_H

#include <cstdint>

namespace backend_core
{
    namespace v1
    {
        /**
         * @brief Represents a symbol for a route, including service, module, and function identifiers.
         * 
         */
        struct RouteSymbol
        {
            std::uint32_t serviceId = 0;
            std::uint32_t moduleId = 0;
            std::uint32_t funcId = 0;

            /**
             * @brief Equality operator for RouteSymbol, compares serviceId, moduleId, and funcId.
             * 
             * @param other The other RouteSymbol to compare with.
             * @return true If both RouteSymbols have the same serviceId, moduleId, and funcId.
             * @return false Otherwise.
             */
            bool operator==(const RouteSymbol& other) const
            {
                return serviceId == other.serviceId && moduleId == other.moduleId && funcId == other.funcId;
            }
        };
    }
}
#endif // ROUTE_SYMBOL_H