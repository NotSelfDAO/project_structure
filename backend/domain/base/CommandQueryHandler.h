#pragma once
#ifndef COMMAND_QUERY_HANDLER_H
#define COMMAND_QUERY_HANDLER_H

#include <optional>

#include "BaseHandler.h"

namespace backend_core
{
    namespace v1
    {
        namespace domain
        {
            /**
             * @brief Base class for command handlers, which may have side effects and require idempotency and version conflict checks.
             * 
             */
            class CommandHandler : public BaseHandler
            {
            protected:
                /**
                 * @brief Handles the given task request.
                 * 
                 * @param request The task request to be handled.
                 * @return TaskEnvelope The result of the handler execution.
                 */
                TaskEnvelope Handle(const TaskRequest& request) const final
                {
                    if (const auto error = EnsureIdempotency(request); error.has_value())
                    {
                        return BuildErrorEnvelope(request, *error);
                    }
                    if (const auto error = CheckVersionConflict(request); error.has_value())
                    {
                        return BuildErrorEnvelope(request, *error);
                    }
                    return ExecuteCommand(request);
                }

                /**
                 * @brief Ensures idempotency for the given task request.
                 * 
                 * @param request The task request to be checked for idempotency.
                 * @return std::optional<DomainError> An optional domain error if idempotency check fails.
                 */
                virtual std::optional<DomainError> EnsureIdempotency(const TaskRequest& request) const
                {
                    (void)request;
                    return std::nullopt;
                }

                /**
                 * @brief Checks for version conflicts for the given task request.
                 * 
                 * @param request The task request to be checked for version conflicts.
                 * @return std::optional<DomainError> An optional domain error if a version conflict is detected.
                 */
                virtual std::optional<DomainError> CheckVersionConflict(const TaskRequest& request) const
                {
                    (void)request;
                    return std::nullopt;
                }

                /**
                 * @brief Executes the command for the given task request.
                 * 
                 * @param request The task request to be processed.
                 * @return TaskEnvelope The result of the command execution.
                 */
                virtual TaskEnvelope ExecuteCommand(const TaskRequest& request) const = 0;
            };

            /**
             * @brief Base class for query handlers, which do not have side effects and do not require idempotency or version conflict checks.
             * 
             */
            class QueryHandler : public BaseHandler
            {
            protected:
                /**
                 * @brief Handles the given task request.
                 * 
                 * @param request The task request to be handled.
                 * @return TaskEnvelope The result of the handler execution.
                 */
                TaskEnvelope Handle(const TaskRequest& request) const final
                {
                    return ExecuteQuery(request);
                }

                /**
                 * @brief Executes the query for the given task request.
                 * 
                 * @param request The task request to be processed.
                 * @return TaskEnvelope The result of the query execution.
                 */
                virtual TaskEnvelope ExecuteQuery(const TaskRequest& request) const = 0;
            };
        }
    }
}

#endif // COMMAND_QUERY_HANDLER_H
