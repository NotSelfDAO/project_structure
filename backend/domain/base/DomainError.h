#pragma once
#ifndef DOMAIN_ERROR_H
#define DOMAIN_ERROR_H

#include <string>

namespace backend_core
{
    namespace v1
    {
        namespace domain
        {
            /**
             * @brief Enum representing domain error codes.
             */
            enum class DomainErrorCode
            {
                InvalidArgument,
                Unauthorized,
                NotFound,
                Conflict,
                InternalError,
                NotImplemented,
            };

            /**
             * @brief Struct representing a domain error.
             */
            struct DomainError
            {
                DomainErrorCode code = DomainErrorCode::InternalError;
                std::string message = "internal_error";
                bool retriable = false;
            };

            /**
             * @brief Converts a domain error code to its string representation.
             * 
             * @param code The domain error code to be converted.
             * @return const char* The string representation of the domain error code.
             */
            inline const char* ToString(DomainErrorCode code)
            {
                switch (code)
                {
                case DomainErrorCode::InvalidArgument:
                    return "invalid_argument";
                case DomainErrorCode::Unauthorized:
                    return "unauthorized";
                case DomainErrorCode::NotFound:
                    return "not_found";
                case DomainErrorCode::Conflict:
                    return "conflict";
                case DomainErrorCode::InternalError:
                    return "internal_error";
                case DomainErrorCode::NotImplemented:
                    return "not_implemented";
                default:
                    return "unknown_error";
                }
            }
        }
    }
}

#endif // DOMAIN_ERROR_H
