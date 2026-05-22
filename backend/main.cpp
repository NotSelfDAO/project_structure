#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "core/registrar/AutoRegistrar.h"
#include "core/registrar/HierarchicalRegistry.h"
#include "core/route/SymbolTable.h"
#include "core/task_manager/BackendTask.h"
#include "core/task_manager/BackendTaskPool.h"
#include "core/transport/HttpTaskServer.h"

namespace {
constexpr const char* kHttpHost = "0.0.0.0";
constexpr int kHttpPort = 8080;
constexpr const char* kSmokeTestFlag = "--smoke-test";

backend_core::v1::TaskEnvelope HealthPingHandler(const backend_core::v1::TaskRequest& request)
{
   backend_core::v1::TaskEnvelope envelope;
   envelope.taskId = request.taskId;
   envelope.route = request.route;
   envelope.success = true;
   envelope.message = "pong";
   envelope.processedAt = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()).count();
   return envelope;
}

AUTO_REGISTER("system", "health", "ping", HealthPingHandler);

bool RunSmokeTest()
{
   backend_core::v1::SymbolTable symbols;
   backend_core::v1::HierarchicalRegistry registry;

   backend_core::v1::RegistryBootstrap::RegisterAll(registry, symbols);

   backend_core::v1::TaskRequest request;
   request.taskId = "smoke_test";
   request.route.serviceName = "system";
   request.route.moduleName = "health";
   request.route.funcName = "ping";

   const auto envelope = registry.Dispatch(request, symbols);
   std::cout << "smoke_test success=" << envelope.success
             << " message=" << envelope.message << std::endl;
   return envelope.success && envelope.message == "pong";
}
}

int main(int argc, char* argv[])
{
   if (argc > 1 && std::string(argv[1]) == kSmokeTestFlag)
   {
      return RunSmokeTest() ? 0 : 1;
   }

   backend_core::v1::SymbolTable symbols;
   backend_core::v1::HierarchicalRegistry registry;
   backend_core::v1::BackendTaskPool taskPool;
   backend_core::v1::HttpTaskServer httpServer(registry, symbols, taskPool);

   taskPool.Start();

   backend_core::v1::RegistryBootstrap::RegisterAll(registry, symbols);

   if (!httpServer.Start(kHttpHost, kHttpPort))
   {
      std::cerr << "Failed to start HTTP server on " << kHttpHost << ':' << kHttpPort << std::endl;
      taskPool.Stop();
      return 1;
   }

   std::cout << "HTTP transport listening on " << kHttpHost << ':' << kHttpPort << std::endl;
   std::cout << "POST /api/v1/tasks/execute with route system/health/ping" << std::endl;
   std::cout << "Press Enter to stop the backend." << std::endl;

   std::atomic<bool> running = true;
   std::thread worker([&]() {
      while (running.load())
      {
         backend_core::v1::BackendTask task;
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