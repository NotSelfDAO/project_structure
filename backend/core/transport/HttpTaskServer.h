#pragma once
#ifndef HTTP_TASK_SERVER_H
#define HTTP_TASK_SERVER_H

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "../registrar/HierarchicalRegistry.h"
#include "../route/SymbolTable.h"
#include "../task_manager/BackendTaskPool.h"

namespace httplib {
class Server;
}

namespace backend_core
{
    namespace v1
    {
        class HttpTaskServer
        {
        public:
            HttpTaskServer(
                HierarchicalRegistry& registry,
                SymbolTable& symbols,
                BackendTaskPool& taskPool);
            ~HttpTaskServer();

            bool Start(const std::string& host, int port);
            void Stop();

        private:
            HierarchicalRegistry& registry_;
            SymbolTable& symbols_;
            BackendTaskPool& taskPool_;

            std::unique_ptr<httplib::Server> server_;
            std::thread serverThread_;
            std::atomic<bool> running_{false};
        };
    }
}

#endif // HTTP_TASK_SERVER_H
