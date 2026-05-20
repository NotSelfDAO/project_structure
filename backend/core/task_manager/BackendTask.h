#pragma once
#ifndef BACKEND_TASK_H
#define BACKEND_TASK_H

#include <cstdint>
#include <string>

#include "TaskModel.h"

namespace backend_core
{
    namespace v1
    {
        struct BackendTask
        {
            std::int64_t receivedAtSec = 0;
            std::string taskId;
            RouteKey route;
            TaskRequest request;
        };
    }
}

#endif // BACKEND_TASK_H