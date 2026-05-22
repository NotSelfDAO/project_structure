#pragma once
#ifndef TASK_MODEL_H
#define TASK_MODEL_H

#include <vector>
#include <cstdint>
#include <string>

#include "../route/RouteKey.h"

namespace backend_core
{
    namespace v1
    {
        /**
         * @brief ContentItem represents the content of a task. It can be used to store the input and output of a task. 
          The uuid is a unique identifier for the content item, and it can be used to track the content across different tasks. 
          The title and body are the main content of the item, and they can be used to store any information related to the task. 
          The updatedAt is the timestamp when the content was last updated, and it can be used for version control. 
          The version is a number that represents the version of the content, and it can be used for optimistic concurrency control.
         * 
         */
        struct ContentItem
        {
            std::string uuid;
            std::string title;
            std::string body;
            std::int64_t updatedAt = 0;
            std::int64_t version = 0;
        };

        /**
         * @brief TaskRequest represents a request to create a task. It contains the task ID, route, and content. 
          The task ID is a unique identifier for the task, and it can be used to track the task across different systems. 
          The route is used to identify which function the task belongs to, and the content is the input of the function. 
          The retryCount is used to track how many times the task has been retried, and it can be used for retry logic.
         * 
         */
        struct TaskRequest
        {
            std::string taskId;
            RouteKey route;
            ContentItem content;
            std::int64_t retryCount = 0;
        };

        /**
         * @brief TaskEnvelope represents the result of a task. It contains the task ID, route, success status, message, and processed timestamp.
         *        It can also contain the output items of the task.
         */
        struct TaskEnvelope
        {
            std::string taskId;
            RouteKey route;
            bool success = false;
            std::string message;
            std::int64_t processedAt = 0;

            bool hasItem = false;
            ContentItem item;
            std::vector<ContentItem> items;
        };
    }
}



#endif // TASK_MODEL_H
