#pragma once
#ifndef ENVELOPE_BUILDERS_H
#define ENVELOPE_BUILDERS_H

#include <chrono>
#include <string>
#include <vector>

#include "../../core/task_manager/TaskModel.h"
#include "DomainError.h"

namespace backend_core
{
    namespace v1
    {
        namespace domain
        {
            /**
             * @brief Gets the current Unix time in seconds.
             * 
             * @return std::int64_t The current Unix time in seconds.
             */
            inline std::int64_t CurrentUnixTimeSec()
            {
                return std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            }

            /**
             * @brief Builds a success envelope for the given task request.
             * 
             * @param request The task request to be processed.
             * @param message The success message.
             * @return TaskEnvelope The success envelope.
             */
            inline TaskEnvelope BuildSuccessEnvelope(const TaskRequest& request, const std::string& message = "ok")
            {
                TaskEnvelope envelope;
                envelope.taskId = request.taskId;
                envelope.route = request.route;
                envelope.success = true;
                envelope.message = message;
                envelope.processedAt = CurrentUnixTimeSec();
                return envelope;
            }

            /**
             * @brief Builds a success envelope with a single item for the given task request.
             * 
             * @param request The task request to be processed.
             * @param item The content item to be included in the envelope.
             * @param message The success message.
             * @return TaskEnvelope The success envelope with the item.
             */
            inline TaskEnvelope BuildSuccessWithItemEnvelope(const TaskRequest& request, const ContentItem& item, const std::string& message = "ok")
            {
                TaskEnvelope envelope = BuildSuccessEnvelope(request, message);
                envelope.hasItem = true;
                envelope.item = item;
                return envelope;
            }

            /**
             * @brief Builds a success envelope with multiple items for the given task request.
             * 
             * @param request The task request to be processed.
             * @param items The content items to be included in the envelope.
             * @param message The success message.
             * @return TaskEnvelope The success envelope with the items.
             */
            inline TaskEnvelope BuildSuccessWithItemsEnvelope(const TaskRequest& request, const std::vector<ContentItem>& items, const std::string& message = "ok")
            {
                TaskEnvelope envelope = BuildSuccessEnvelope(request, message);
                envelope.items = items;
                return envelope;
            }

            /**
             * @brief Builds an error envelope for the given task request.
             * 
             * @param request The task request to be processed.
             * @param error The domain error to be included in the envelope.
             * @return TaskEnvelope The error envelope.
             */
            inline TaskEnvelope BuildErrorEnvelope(const TaskRequest& request, const DomainError& error)
            {
                TaskEnvelope envelope;
                envelope.taskId = request.taskId;
                envelope.route = request.route;
                envelope.success = false;
                envelope.message = error.message.empty() ? ToString(error.code) : error.message;
                envelope.processedAt = CurrentUnixTimeSec();
                return envelope;
            }
        }
    }
}

#endif // ENVELOPE_BUILDERS_H
