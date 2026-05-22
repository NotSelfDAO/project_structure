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
        /**
         * @brief HttpTaskServer handles HTTP requests for backend tasks. It interacts with the hierarchical registry, symbol table, and backend task pool to process tasks.
         */
        class HttpTaskServer
        {
        public:
            /**
             * @brief Construct a new Http Task Server object
             * 
             * @param registry The hierarchical registry to be used by the server.
             * @param symbols The symbol table to be used by the server.
             * @param taskPool The backend task pool to be used by the server.
             */
            HttpTaskServer(
                HierarchicalRegistry& registry,
                SymbolTable& symbols,
                BackendTaskPool& taskPool);
            ~HttpTaskServer();

            /**
             * @brief Start the HTTP server on the specified host and port.
             * 
             * @param host The host address to bind the server to (e.g., "0.0.0.0").
             * @param port The port number to bind the server to (e.g., 8080).
             * @return true if the server started successfully, false otherwise.
             */
            bool Start(const std::string& host, int port);
            
            /**
             * @brief Stop the HTTP server.
             */
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
