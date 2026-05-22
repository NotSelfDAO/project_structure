#include "BackendRuntime.h"

#include <atomic>
#include <iostream>
#include <string>
#include <thread>

#include "../registrar/AutoRegistrar.h"
#include "../registrar/HierarchicalRegistry.h"
#include "../route/SymbolTable.h"
#include "../task_manager/BackendTask.h"
#include "../task_manager/BackendTaskPool.h"
#include "../transport/HttpTaskServer.h"

namespace
{
constexpr const char* kHttpHost = "0.0.0.0";
constexpr int kHttpPort = 8080;
constexpr const char* kSmokeTestFlag = "--smoke-test";

bool RunSmokeTest()
{
    backend_core::v1::SymbolTable symbols;
    backend_core::v1::HierarchicalRegistry registry;

    backend_core::v1::RegistryBootstrap::RegisterAll(registry, symbols);

    backend_core::v1::TaskRequest request;
    request.taskId = "smoke_test";
    request.route.serviceName = "template";
    request.route.moduleName = "read";
    request.route.funcName = "get";

    const auto envelope = registry.Dispatch(request, symbols);
    std::cout << "smoke_test success=" << envelope.success
              << " message=" << envelope.message << std::endl;
    return envelope.success && envelope.message == "read_template_ok";
}
}

namespace backend_core
{
    namespace v1
    {
        int RunBackend(int argc, char* argv[])
        {
            if (argc > 1 && std::string(argv[1]) == kSmokeTestFlag)
            {
                return RunSmokeTest() ? 0 : 1;
            }

            SymbolTable symbols;
            HierarchicalRegistry registry;
            BackendTaskPool taskPool;
            HttpTaskServer httpServer(registry, symbols, taskPool);

            taskPool.Start();
            RegistryBootstrap::RegisterAll(registry, symbols);

            if (!httpServer.Start(kHttpHost, kHttpPort))
            {
                std::cerr << "Failed to start HTTP server on " << kHttpHost << ':' << kHttpPort << std::endl;
                taskPool.Stop();
                return 1;
            }

            std::cout << "HTTP transport listening on " << kHttpHost << ':' << kHttpPort << std::endl;
            std::cout << "POST /api/v1/tasks/execute with route template/read/get" << std::endl;
            std::cout << "Press Enter to stop the backend." << std::endl;

            std::atomic<bool> running = true;
            std::thread worker([&]() {
                while (running.load())
                {
                    BackendTask task;
                    if (!taskPool.WaitAndPopTask(task))
                    {
                        break;
                    }

                    const auto envelope = registry.Dispatch(task.request, symbols);
                    std::cout << "task=" << envelope.taskId
                              << " success=" << envelope.success
                              << " msg=" << envelope.message << std::endl;
                }
            });

            std::string line;
            std::getline(std::cin, line);

            running = false;
            httpServer.Stop();
            taskPool.Stop();
            if (worker.joinable())
            {
                worker.join();
            }

            return 0;
        }
    }
}
