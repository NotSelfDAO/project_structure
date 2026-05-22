# project_structure

[中文说明](README.zh-CN.md)

## Overview

This repository is a backend project structure playground focused on a minimal but extensible task-routing framework.

The current implementation centers on the `backend/` folder and provides:

- a C++ backend runtime entrypoint
- hierarchical route registration with `service/module/function`
- synchronous task execution and asynchronous task submission
- auto-registration for business handlers
- read/write business handler templates for follow-up development
- Lua scripts for project scaffolding and CMake file generation

At the workspace level, the repository currently contains:

- `backend/`: the active C++ backend framework and generator scripts
- `frontend/`: reserved for future frontend work
- `backend_ninja_build_script.ps1`: a simple local script entry for Lua-driven generation

## Repository Layout

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

## Backend Highlights

### Clean entrypoint

`backend/main.cpp` is kept minimal and framework-only. Runtime startup is delegated to `backend/core/app/BackendRuntime.cpp`.

### Route-based handler dispatch

Requests are routed by three fields:

- `serviceName`
- `moduleName`
- `funcName`

Handlers are registered through `AUTO_REGISTER(...)` and loaded during startup by `RegistryBootstrap::RegisterAll(...)`.

### Business development templates

Business logic is intentionally constrained into two shapes:

- read handlers based on `QueryHandler`
- write handlers based on `CommandHandler`

This keeps follow-up development consistent and prevents business logic from leaking into the runtime entrypoint.

## Current Template Routes

The current minimal templates are:

- read template: `template/read/get`
- write template: `template/write/upsert`

The smoke test validates the read template route.

## Build And Run

### Configure and build

From the repository root:

```powershell
Push-Location .\backend
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j 8
Pop-Location
```

### Run smoke test

```powershell
Push-Location .\backend
.\build\bin\backend.exe --smoke-test
Pop-Location
```

Expected output:

```text
smoke_test success=1 message=read_template_ok
```

### Run the server

```powershell
Push-Location .\backend
.\build\bin\backend.exe
Pop-Location
```

Default listening address:

- host: `0.0.0.0`
- port: `8080`

## API Quick Start

### Health check

```bash
curl -X GET http://127.0.0.1:8080/api/v1/health
```

### Execute read template synchronously

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

### Submit write template asynchronously

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

## How To Add Business Logic

Recommended workflow:

1. Include `backend/core/api/BackendDev.h`.
2. Copy a handler template from `backend/domain/handlers/`.
3. For read logic, inherit from `QueryHandler`.
4. For write logic, inherit from `CommandHandler`.
5. Implement `Validate(...)` when input checks are needed.
6. Implement `ExecuteQuery(...)` or `ExecuteCommand(...)`.
7. Register the handler with `AUTO_REGISTER(...)` using a plain wrapper entry function.

## Documentation

Detailed documents are available under `backend/docs/`:

- `backend/docs/backend_framework_overview.md`
- `backend/docs/backend_business_handler_rules.md`

Recommended reading order:

1. `backend/docs/backend_framework_overview.md`
2. `backend/docs/backend_business_handler_rules.md`

## Notes

- `frontend/` is currently a placeholder area and is not the active focus of this repository.
- The backend templates are intentionally minimal; they are designed as a starting point for real business handlers.
- If the linker reports `backend.exe` permission errors during rebuild, close any running backend process and rebuild again.
