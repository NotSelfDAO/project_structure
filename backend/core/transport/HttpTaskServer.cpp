#include "HttpTaskServer.h"

#include <chrono>
// #include <sstream>
#include <string>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "../task_manager/BackendTask.h"

namespace backend_core
{
    namespace v1
    {
        namespace
        {
            using json = nlohmann::json;

            std::string NewTaskId()
            {
                const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
                const auto ticks = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
                return "http_" + std::to_string(ticks);
            }

            bool ParseTaskRequest(const json& payload, TaskRequest& request, std::string& error)
            {
                if (!payload.contains("serviceName") || !payload["serviceName"].is_string())
                {
                    error = "serviceName_required";
                    return false;
                }
                if (!payload.contains("moduleName") || !payload["moduleName"].is_string())
                {
                    error = "moduleName_required";
                    return false;
                }
                if (!payload.contains("funcName") || !payload["funcName"].is_string())
                {
                    error = "funcName_required";
                    return false;
                }

                request.taskId = payload.value("taskId", "");
                if (request.taskId.empty())
                {
                    request.taskId = NewTaskId();
                }

                request.route.serviceName = payload["serviceName"].get<std::string>();
                request.route.moduleName = payload["moduleName"].get<std::string>();
                request.route.funcName = payload["funcName"].get<std::string>();
                request.retryCount = payload.value("retryCount", static_cast<std::int64_t>(0));

                if (payload.contains("content") && payload["content"].is_object())
                {
                    const auto& c = payload["content"];
                    request.content.uuid = c.value("uuid", "");
                    request.content.title = c.value("title", "");
                    request.content.body = c.value("body", "");
                    request.content.updatedAt = c.value("updatedAt", static_cast<std::int64_t>(0));
                    request.content.version = c.value("version", static_cast<std::int64_t>(0));
                }

                return true;
            }

            json EnvelopeToJson(const TaskEnvelope& envelope)
            {
                json out;
                out["taskId"] = envelope.taskId;
                out["success"] = envelope.success;
                out["message"] = envelope.message;
                out["processedAt"] = envelope.processedAt;
                out["route"] = {
                    {"serviceName", envelope.route.serviceName},
                    {"moduleName", envelope.route.moduleName},
                    {"funcName", envelope.route.funcName},
                };

                if (envelope.hasItem)
                {
                    out["item"] = {
                        {"uuid", envelope.item.uuid},
                        {"title", envelope.item.title},
                        {"body", envelope.item.body},
                        {"updatedAt", envelope.item.updatedAt},
                        {"version", envelope.item.version},
                    };
                }

                out["items"] = json::array();
                for (const auto& item : envelope.items)
                {
                    out["items"].push_back({
                        {"uuid", item.uuid},
                        {"title", item.title},
                        {"body", item.body},
                        {"updatedAt", item.updatedAt},
                        {"version", item.version},
                    });
                }

                return out;
            }
        }

        HttpTaskServer::HttpTaskServer(HierarchicalRegistry& registry,
                                       SymbolTable& symbols,
                                       BackendTaskPool& taskPool)
            : registry_(registry), symbols_(symbols), taskPool_(taskPool)
        {
        }

        HttpTaskServer::~HttpTaskServer()
        {
            Stop();
        }

        bool HttpTaskServer::Start(const std::string& host, int port)
        {
            if (running_.load())
            {
                return true;
            }

            server_ = std::make_unique<httplib::Server>();

            server_->Get("/api/v1/health", [](const httplib::Request&, httplib::Response& res) {
                res.set_content(R"({"success":true,"message":"ok"})", "application/json");
                res.status = 200;
            });

            server_->Post("/api/v1/tasks/execute",
                [this](const httplib::Request& req, httplib::Response& res)
                {
                    json payload;
                    try
                    {
                        payload = json::parse(req.body);
                    }
                    catch (...)
                    {
                        res.status = 400;
                        res.set_content(R"({"success":false,"message":"invalid_json"})", "application/json");
                        return;
                    }

                    TaskRequest request;
                    std::string error;
                    if (!ParseTaskRequest(payload, request, error))
                    {
                        res.status = 400;
                        res.set_content(json({{"success", false}, {"message", error}}).dump(), "application/json");
                        return;
                    }

                    auto envelope = registry_.Dispatch(request, symbols_);
                    envelope.processedAt = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                    res.status = envelope.success ? 200 : 404;
                    res.set_content(EnvelopeToJson(envelope).dump(), "application/json");
                });

            server_->Post("/api/v1/tasks/submit",
                [this](const httplib::Request& req, httplib::Response& res)
                {
                    json payload;
                    try
                    {
                        payload = json::parse(req.body);
                    }
                    catch (...)
                    {
                        res.status = 400;
                        res.set_content(R"({"accepted":false,"message":"invalid_json"})", "application/json");
                        return;
                    }

                    TaskRequest request;
                    std::string error;
                    if (!ParseTaskRequest(payload, request, error))
                    {
                        res.status = 400;
                        res.set_content(json({{"accepted", false}, {"message", error}}).dump(), "application/json");
                        return;
                    }

                    BackendTask task;
                    task.taskId = request.taskId;
                    task.route = request.route;
                    task.request = request;
                    task.receivedAtSec = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();

                    const bool accepted = taskPool_.WaitAndPushTask(task);
                    res.status = accepted ? 202 : 503;
                    res.set_content(
                        json({
                            {"accepted", accepted},
                            {"taskId", request.taskId},
                            {"message", accepted ? "accepted" : "task_pool_stopped"},
                        }).dump(),
                        "application/json");
                });

            running_ = true;
            serverThread_ = std::thread([this, host, port]() {
                server_->listen(host.c_str(), port);
                running_ = false;
            });

            return true;
        }

        void HttpTaskServer::Stop()
        {
            if (!running_.load())
            {
                if (serverThread_.joinable())
                {
                    serverThread_.join();
                }
                return;
            }

            server_->stop();
            running_ = false;
            if (serverThread_.joinable())
            {
                serverThread_.join();
            }
        }
    }
}
