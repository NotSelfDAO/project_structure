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
}

int main(int argc, char* argv[])
{
   (void)argc;
   (void)argv;
   backend_core::v1::SymbolTable symbols;
   backend_core::v1::HierarchicalRegistry registry;
   backend_core::v1::BackendTaskPool taskPool;
   backend_core::v1::HttpTaskServer httpServer(registry, symbols, taskPool);

   taskPool.Start();

   backend_core::v1::RegistryBootstrap::RegisterAll(registry, symbols);

   httpServer.Start("0.0.0.0", 8080);
   std::cout << "HTTP transport listening on 0.0.0.0:8080" << std::endl;
   std::cout << "POST /api/v1/tasks/execute with route system/health/ping" << std::endl;

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