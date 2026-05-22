# 后端框架运行流程与扩展说明

## 1. 这套后端框架现在在做什么

这套框架的定位不是“业务实现本体”，而是一个可生成项目骨架、承载任务路由、接收 HTTP 请求、分发任务执行、并支持自动注册 handler 的后端基础设施。

它当前已经具备以下能力：

- 自动生成项目目录结构和 CMakeLists.txt
- 提供 HTTP 接口接收任务请求
- 提供同步执行与异步入队两条任务链路
- 提供 service / module / function 三层路由索引
- 提供自动注册机制，把业务 handler 从源码静态收集到注册表中
- 提供基础任务模型、内容模型和路由模型

它当前还没有完成的部分主要是：

- 真实业务 handler 的系统化填充
- `build / compile / debug / run` 这些菜单项对应的完整工程动作
- 更完善的错误处理、日志和测试体系
- 对外暴露更丰富的业务 API 和持久化能力

## 2. `main.cpp` 的运行流程

入口文件是 [backend/main.cpp](backend/main.cpp)。它的职责是把框架各层串起来，然后启动服务。

### 2.1 启动阶段

`main.cpp` 先包含了自动注册、注册表、符号表、任务池和 HTTP server 的头文件，然后定义了一个示例 handler：

- `HealthPingHandler`
- 路由是 `system / health / ping`
- 通过 `AUTO_REGISTER("system", "health", "ping", HealthPingHandler)` 注册到自动注册表

这个 handler 的逻辑很简单：收到请求后，返回一份 `pong` 的响应，并回填 `taskId`、`route` 和 `processedAt`。

### 2.2 构造核心运行对象

`main()` 中依次创建了四个核心对象：

- `SymbolTable symbols`
- `HierarchicalRegistry registry`
- `BackendTaskPool taskPool`
- `HttpTaskServer httpServer(registry, symbols, taskPool)`

这四个对象分别承担：

- `SymbolTable`：把字符串路由内部化成 ID
- `HierarchicalRegistry`：按层级路由存放 handler
- `BackendTaskPool`：管理异步任务队列
- `HttpTaskServer`：对外暴露 HTTP 接口

### 2.3 启动任务池

`taskPool.Start()` 会把任务池置为可运行状态，允许后续接收任务。

### 2.4 注册所有自动注册的 handler

`backend_core::v1::RegistryBootstrap::RegisterAll(registry, symbols);` 会把所有通过 `AUTO_REGISTER` 收集到的 handler 统一注册到 `registry` 中。

这一步很关键，因为 `AUTO_REGISTER` 只是“收集”，真正变成可调度路由是在这里完成的。

### 2.5 启动 HTTP 服务

`httpServer.Start("0.0.0.0", 8080);` 之后，HTTP 服务监听 8080 端口。

当前代码中打印了两条提示：

- HTTP 服务已启动
- 可用 `POST /api/v1/tasks/execute` 调用 `system/health/ping`

### 2.6 启动后台 worker 线程

`main.cpp` 还启动了一个 worker 线程：

- 线程循环等待 `taskPool.WaitAndPopTask(task)`
- 取出任务后调用 `registry.Dispatch(task.request, symbols)`
- 打印执行结果

这表示框架同时支持：

- HTTP 直接执行
- HTTP 入队，后台线程消费执行

### 2.7 退出流程

主线程通过 `std::getline(std::cin, line);` 阻塞等待输入。

当收到输入后：

- `running = false`
- `httpServer.Stop()`
- `taskPool.Stop()`
- `worker.join()`

最后退出程序。

## 3. 请求在框架里的流转路径

### 3.1 同步执行链路

如果请求走 `POST /api/v1/tasks/execute`，大致流程是：

1. HTTP Server 接收 JSON 请求
2. 解析出 `serviceName`、`moduleName`、`funcName`
3. 通过 `SymbolTable` 找到对应 ID
4. 通过 `HierarchicalRegistry` 找到 handler
5. 执行 handler
6. 返回 `TaskEnvelope` 的 JSON 结果

### 3.2 异步入队链路

如果请求走 `POST /api/v1/tasks/submit`，流程是：

1. HTTP Server 接收 JSON 请求
2. 解析出任务内容
3. 封装成 `BackendTask`
4. 放入 `BackendTaskPool`
5. 后台 worker 线程从队列取出任务
6. 通过 `registry.Dispatch` 执行
7. 打印执行结果

### 3.3 健康检查链路

`GET /api/v1/health` 直接返回固定 JSON：

- `success: true`
- `message: ok`

这是框架最基础的可用性检查接口。

## 4. 核心模块完成情况

### 4.1 已完成的部分

#### 4.1.1 路由与调度骨架

[backend/core/route/SymbolTable.h](backend/core/route/SymbolTable.h) 和 [backend/core/route/SymbolTable.cpp](backend/core/route/SymbolTable.cpp) 已经提供了：

- service / module / function 的符号表
- 路由 ID 的内部化
- 反查接口

[backend/core/registrar/HierarchicalRegistry.h](backend/core/registrar/HierarchicalRegistry.h) 和 [backend/core/registrar/HierarchicalRegistry.cpp](backend/core/registrar/HierarchicalRegistry.cpp) 已经提供了：

- 路由注册
- 按名称注册
- 路由分发
- 未命中时的失败 envelope

#### 4.1.2 自动注册机制

[backend/core/registrar/AutoRegistrar.h](backend/core/registrar/AutoRegistrar.h) 和 [backend/core/registrar/AutoRegistrar.cpp](backend/core/registrar/AutoRegistrar.cpp) 已经提供了：

- `AUTO_REGISTER` 宏
- 自动注册收集器
- 启动时统一注册入口

这意味着你只要在任意业务 cpp 里写：

```cpp
AUTO_REGISTER("system", "health", "ping", HealthPingHandler);
```

就能把 handler 收集起来，再由 `RegistryBootstrap::RegisterAll` 在启动时统一装配。

#### 4.1.3 任务模型

[backend/core/task_manager/TaskModel.h](backend/core/task_manager/TaskModel.h) 已经定义了：

- `ContentItem`
- `TaskRequest`
- `TaskEnvelope`

这为 HTTP 输入输出和内部调度提供了统一数据结构。

#### 4.1.4 任务池

[backend/core/task_manager/BackendTaskPool.h](backend/core/task_manager/BackendTaskPool.h) 和 [backend/core/task_manager/BackendTaskPool.cpp](backend/core/task_manager/BackendTaskPool.cpp) 已经提供了：

- 启动 / 停止控制
- 阻塞式入队
- 阻塞式出队
- 条件变量唤醒

这是异步任务链路的基础。

#### 4.1.5 HTTP 服务壳

[backend/core/transport/HttpTaskServer.h](backend/core/transport/HttpTaskServer.h) 和 [backend/core/transport/HttpTaskServer.cpp](backend/core/transport/HttpTaskServer.cpp) 已经提供了：

- `/api/v1/health`
- `/api/v1/tasks/execute`
- `/api/v1/tasks/submit`
- JSON 解析和 envelope 输出
- 与注册表和任务池的连接

#### 4.1.6 工程生成脚本

`scripts/` 目录下的 Lua 生成脚本已经可以：

- 初始化目录结构
- 生成根目录 CMakeLists.txt
- 生成 core / domain 的 CMakeLists.txt
- 生成第三方配置文件
- 复制 core 模板代码

也就是说，框架的“项目生成器”这一层已经基本成型。

### 4.2 还没有完成的部分

#### 4.2.1 业务模块仍然是骨架

目前 `main.cpp` 里只有一个示例 handler：

- `system/health/ping`

这说明“路由机制”完成了，但“具体业务内容”还没系统填充。

#### 4.2.2 `run` / `debug` / `build` 菜单还没真正做完

Lua 菜单里已经预留了这几个入口，但其中不少函数仍然只是占位或半成品。

#### 4.2.3 工程化细节还需要收尾

还包括：

- 路径与命令拼接的健壮性
- Windows / Linux 下命令兼容性
- 错误提示统一化
- 构建缓存清理和重建策略
- 自动化测试

## 5. 如何在这个框架上填充具体内容

### 5.1 新增一个业务接口的标准做法

如果你要新增一个业务功能，建议按下面步骤做：

1. 先定义 handler 函数
2. 用 `AUTO_REGISTER` 注册路由
3. 在 `main.cpp` 启动时调用 `RegistryBootstrap::RegisterAll`
4. 从 HTTP 接口调用该路由
5. 如有需要，再把它接到异步任务队列里

### 5.2 建议的开发顺序

#### 第一步：明确路由命名

建议沿用三层结构：

- `serviceName`
- `moduleName`
- `funcName`

例如：

- `user/auth/login`
- `user/profile/get`
- `order/cart/add`

这样和现有 `SymbolTable` / `HierarchicalRegistry` 的设计完全一致。

#### 第二步：定义输入输出

把接口的请求字段放进 `TaskRequest::content`，把返回结果放进 `TaskEnvelope`：

- `TaskRequest`：输入
- `TaskEnvelope`：输出

如果返回单个实体，用 `TaskEnvelope::item`。
如果返回列表，用 `TaskEnvelope::items`。

#### 第三步：实现 handler

handler 只负责一件事：

- 接收 `TaskRequest`
- 执行业务逻辑
- 返回 `TaskEnvelope`

建议把复杂逻辑拆到独立 service 层，不要把所有代码都塞进 handler。

#### 第四步：注册 handler

使用：

```cpp
AUTO_REGISTER("service", "module", "func", YourHandler);
```

然后在程序启动时统一注册。

#### 第五步：决定走同步还是异步

- 如果接口需要立即返回结果，用 `/api/v1/tasks/execute`
- 如果接口适合排队处理，用 `/api/v1/tasks/submit`

### 5.3 一个推荐的业务填充模板

你可以按下面的分层来补内容：

- `core/registrar`：只做注册，不写业务
- `core/route`：只做路由映射，不写业务
- `core/task_manager`：只做任务调度，不写业务
- `core/transport`：只做 HTTP 接入，不写业务
- `domain`：放真实业务规则和数据模型
- `docs`：记录接口和设计

这样框架会保持干净，不会退化成“所有逻辑都塞在 main.cpp” 的状态。

## 6. 这份代码当前需要注意的几个点

### 6.1 `main.cpp` 里有一个明显的调用错误风险

这条风险已经在当前代码里修正完毕。现在 `main.cpp` 已经补齐了：

- `path_analyse.get_current_project_path()` 的正确调用
- HTTP 服务启动失败时的显式退出
- `--smoke-test` 非交互最小验证入口

如果你回头看旧版本，曾经的问题是这一行：

```cpp
params.repo_root_path = path_analyse.get_current_project_path
```

少了括号，应该是函数调用，不是函数对象。

如果这段代码还没改，初始化时会把函数值传下去，导致后续参数类型错误。

现在这部分已经修正，并且已经通过最小可运行测试：

```text
smoke_test success=1 message=pong
```

### 6.2 构建脚本和 CMake 需要清理旧缓存

如果你之前换过项目路径，`FetchContent` 的子构建缓存可能会带着旧绝对路径，需要清理 `third_party/*-subbuild` 目录后重配。

### 6.3 `build` / `compile` 命令需要注意路径引号

当前脚本里有一些命令拼接直接拼路径的情况，如果项目目录带空格，会出问题。

## 7. 总结

一句话概括：

这套后端框架的“运行骨架”已经完成，已经能启动 HTTP 服务、注册路由、接收任务、分发任务、并生成项目工程文件；但“具体业务层”和“工程化收尾”还需要继续填充。

如果你下一步要真正让它可用，最值得优先补的顺序是：

1. 继续补真实业务 handler，而不是只保留 `system/health/ping`
2. 补一两个真实业务 handler
3. 把业务 handler 接入 `AUTO_REGISTER`
4. 补全 build / compile / debug 的菜单动作
5. 加一组最小可运行测试
