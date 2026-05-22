# project_structure

[English README](README.md)

## 项目简介

这个仓库是一个以后端为核心的项目结构实验仓库，当前重点是 `backend/` 目录中的最小可扩展任务路由框架。

当前已经具备：

- C++ 后端运行时入口
- `service/module/function` 三层路由分发
- 同步执行任务与异步提交任务两条链路
- 基于 `AUTO_REGISTER` 的业务自动注册机制
- 面向后续开发者的读模板 / 写模板
- 基于 Lua 的项目结构与 CMake 生成脚本

当前仓库主要目录：

- `backend/`：当前主要开发目标，包含后端框架与生成脚本
- `frontend/`：预留给后续前端开发
- `backend_ninja_build_script.ps1`：本地 Lua 脚本入口

## 目录结构

```text
project_structure/
├─ backend/
│  ├─ core/
│  │  ├─ api/
│  │  ├─ app/
│  │  ├─ registrar/
│  │  ├─ route/
│  │  ├─ task_manager/
│  │  └─ transport/
│  ├─ domain/
│  │  ├─ base/
│  │  └─ handlers/
│  ├─ docs/
│  ├─ scripts/
│  └─ CMakeLists.txt
├─ frontend/
└─ backend_ninja_build_script.ps1
```

## 后端框架特点

### 干净的入口文件

`backend/main.cpp` 现在保持为纯框架入口，不承载业务逻辑。实际运行时启动流程放在 `backend/core/app/BackendRuntime.cpp`。

### 基于路由的任务分发

请求通过以下三个字段进行分发：

- `serviceName`
- `moduleName`
- `funcName`

业务处理函数通过 `AUTO_REGISTER(...)` 注册，并在启动阶段由 `RegistryBootstrap::RegisterAll(...)` 统一装配。

### 面向业务开发的两类模板

为了让后续开发统一规范，当前业务逻辑被约束成两类：

- 基于 `QueryHandler` 的读操作模板
- 基于 `CommandHandler` 的写操作模板

这样后续开发时，业务逻辑不会再混进运行时入口文件。

## 当前模板路由

当前最小模板路由如下：

- 读模板：`template/read/get`
- 写模板：`template/write/upsert`

其中 smoke test 当前验证的是读模板。

## 构建与运行

### 配置并构建

在仓库根目录执行：

```powershell
Push-Location .\backend
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j 8
Pop-Location
```

### 运行 smoke test

```powershell
Push-Location .\backend
.\build\bin\backend.exe --smoke-test
Pop-Location
```

预期输出：

```text
smoke_test success=1 message=read_template_ok
```

### 启动服务

```powershell
Push-Location .\backend
.\build\bin\backend.exe
Pop-Location
```

默认监听地址：

- host：`0.0.0.0`
- port：`8080`

## 接口快速示例

### 健康检查

```bash
curl -X GET http://127.0.0.1:8080/api/v1/health
```

### 同步执行读模板

```bash
curl -X POST http://127.0.0.1:8080/api/v1/tasks/execute \
  -H "Content-Type: application/json" \
  -d '{
    "taskId": "demo_read_001",
    "serviceName": "template",
    "moduleName": "read",
    "funcName": "get"
  }'
```

### 异步提交写模板

```bash
curl -X POST http://127.0.0.1:8080/api/v1/tasks/submit \
  -H "Content-Type: application/json" \
  -d '{
    "taskId": "demo_write_001",
    "serviceName": "template",
    "moduleName": "write",
    "funcName": "upsert"
  }'
```

## 如何补充业务逻辑

推荐流程：

1. 引入 `backend/core/api/BackendDev.h`
2. 从 `backend/domain/handlers/` 复制一个模板文件
3. 读逻辑继承 `QueryHandler`
4. 写逻辑继承 `CommandHandler`
5. 需要输入校验时实现 `Validate(...)`
6. 实现 `ExecuteQuery(...)` 或 `ExecuteCommand(...)`
7. 用普通入口函数 + `AUTO_REGISTER(...)` 完成注册

## 文档入口

详细文档位于 `backend/docs/`：

- `backend/docs/backend_framework_overview.md`
- `backend/docs/backend_business_handler_rules.md`

建议阅读顺序：

1. `backend/docs/backend_framework_overview.md`
2. `backend/docs/backend_business_handler_rules.md`

## 说明

- `frontend/` 当前还是预留目录，不是本仓库现阶段重点。
- 当前业务模板是故意保持最小化的，目的是作为后续真实业务的统一起点。
- 如果重新构建时遇到 `backend.exe` 被占用导致的权限错误，先关闭正在运行的后端进程，再重新构建即可。
