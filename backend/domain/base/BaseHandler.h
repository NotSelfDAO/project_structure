#pragma once
#ifndef BASE_HANDLER_H
#define BASE_HANDLER_H

#include <optional>

#include "DomainError.h"
#include "EnvelopeBuilders.h"

namespace backend_core
{
    namespace v1
    {
        namespace domain
        {
            /**
             * @brief Base class for all domain handlers.
             * 
             */
            class BaseHandler
            {
            public:
                virtual ~BaseHandler() = default;

                /**
                 * @brief Invokes the handler with the given request.
                 * 
                 * @param request The task request to be processed.
                 * @return TaskEnvelope The result of the handler execution.
                 */
                TaskEnvelope operator()(const TaskRequest& request) const
                {
                    if (const auto error = Validate(request); error.has_value())
                    {
                        return BuildErrorEnvelope(request, *error);
                    }
                    return Handle(request);
                }

            protected:
                /**
                 * @brief Validates the given task request.
                 * 
                 * @param request The task request to be validated.
                 * @return std::optional<DomainError> An optional domain error if validation fails.
                 */
                virtual std::optional<DomainError> Validate(const TaskRequest& request) const
                {
                    (void)request;
                    return std::nullopt;
                }

                /**
                 * @brief Handles the given task request.
                 * 
                 * @param request The task request to be handled.
                 * @return TaskEnvelope The result of the handler execution.
                 */
                virtual TaskEnvelope Handle(const TaskRequest& request) const = 0;
            };

            /**
             * @brief Invokes the framework handler with the given request.
             * 
             * @tparam THandler The type of the handler to be invoked.
             * @param request The task request to be processed.
             * @return TaskEnvelope The result of the handler execution.
             */
            template <typename THandler>
            TaskEnvelope InvokeFrameworkHandler(const TaskRequest& request)
            {
                static THandler handler;
                return handler(request);
            }
        }
    }
}

#endif // BASE_HANDLER_H
