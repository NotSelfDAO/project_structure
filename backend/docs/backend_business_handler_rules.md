# 后端业务逻辑接入规则与读写模板说明

## 0. 3分钟上手清单

第一次接手这个项目，先按下面做即可：

1. 读操作复制 `backend/domain/handlers/UserProfileGetHandler.cpp`，写操作复制 `backend/domain/handlers/HealthPingHandler.cpp`。
2. 修改 5 个地方：类名、入口函数名、`AUTO_REGISTER` 的三段路由、`Validate`、`ExecuteQuery` 或 `ExecuteCommand`。
3. 不要改 3 个结构：匿名命名空间、`InvokeFrameworkHandler<YourHandler>(request)`、`AUTO_REGISTER(..., HandleXXXEntry)` 模式。
4. 返回结果只用统一构建器：`BuildSuccessEnvelope` / `BuildSuccessWithItemEnvelope` / `BuildErrorEnvelope`。
5. 构建并验证：`cmake --build build -j 8`，然后运行 `backend.exe --smoke-test`，确认输出 `smoke_test success=1 message=read_template_ok`。

如果你已经按清单跑通，再继续阅读后续章节理解完整设计。

## 1. 文档目的

这份文档专门说明当前项目里“后续开发者应当如何补充业务逻辑”。

目标不是解释整个后端框架，而是把下面这些事情说清楚：

1. 业务 handler 应该放在哪里
2. 为什么要分成读操作和写操作两类模板
3. `HandleWriteTemplateEntry` / `HandleReadTemplateEntry` 这种函数是干什么的
4. 开发者复制模板时到底应该改哪些地方
5. 一套最小完备的业务逻辑接入规范是什么

## 2. 当前目录职责

当前业务接入最相关的目录如下：

- `backend/core/api/BackendDev.h`
- `backend/domain/base/`
- `backend/domain/handlers/`
- `backend/main.cpp`
- `backend/core/app/BackendRuntime.cpp`

职责划分：

1. `backend/main.cpp`
   只保留框架入口，不写业务逻辑。

2. `backend/core/app/BackendRuntime.cpp`
   负责启动运行时、注册 handler、启动 HTTP 服务、执行 smoke test。

3. `backend/core/api/BackendDev.h`
   是开发者统一入口头文件。后续开发者写业务 handler 时，直接 include 这一个头即可。

4. `backend/domain/base/`
   放业务 handler 的基础抽象和统一规则。

5. `backend/domain/handlers/`
   放具体业务 handler。后续新增业务时，原则上只改这里。

## 3. 为什么只保留“读模板”和“写模板”两类示例

后续大部分业务，都可以抽象成两种：

1. 读操作
   例如：查询详情、查询列表、查询状态。

2. 写操作
   例如：创建、更新、删除、提交、确认、审批。

所以当前项目只保留两类最基础样板：

1. 读模板示例
   文件：`backend/domain/handlers/UserProfileGetHandler.cpp`

2. 写模板示例
   文件：`backend/domain/handlers/HealthPingHandler.cpp`

这样做的目的，是让后续开发者不要从“零设计”开始，而是固定按两类模板扩展。

## 4. 当前基础抽象说明

### 4.1 BaseHandler

文件：`backend/domain/base/BaseHandler.h`

职责：

1. 定义统一处理入口 `operator()(request)`
2. 先执行 `Validate(request)`
3. 校验失败时自动返回统一错误 Envelope
4. 校验通过后再进入真正处理函数

可以把它理解成统一流水线：

`Validate -> Handle -> TaskEnvelope`

### 4.2 QueryHandler

文件：`backend/domain/base/CommandQueryHandler.h`

职责：

1. 继承 `BaseHandler`
2. 固定 `Handle()` 的行为为调用 `ExecuteQuery()`
3. 适合无副作用的查询型业务

适用场景：

- get
- list
- search
- query

### 4.3 CommandHandler

文件：`backend/domain/base/CommandQueryHandler.h`

职责：

1. 继承 `BaseHandler`
2. 在执行前预留幂等检查 `EnsureIdempotency()`
3. 在执行前预留版本冲突检查 `CheckVersionConflict()`
4. 最终调用 `ExecuteCommand()`

适用场景：

- create
- update
- delete
- upsert
- submit

### 4.4 DomainError

文件：`backend/domain/base/DomainError.h`

职责：

1. 统一错误码
2. 统一错误消息
3. 统一是否可重试标记

当前错误码包括：

- `InvalidArgument`
- `Unauthorized`
- `NotFound`
- `Conflict`
- `InternalError`
- `NotImplemented`

### 4.5 EnvelopeBuilders

文件：`backend/domain/base/EnvelopeBuilders.h`

职责：

1. 统一成功返回结构
2. 统一失败返回结构
3. 自动补齐 `processedAt`

当前提供的主要函数：

- `BuildSuccessEnvelope`
- `BuildSuccessWithItemEnvelope`
- `BuildSuccessWithItemsEnvelope`
- `BuildErrorEnvelope`
- `CurrentUnixTimeSec`

## 5. 读模板逐步讲解

文件：`backend/domain/handlers/UserProfileGetHandler.cpp`

它当前承担的是“读样板”的角色，而不是固定业务本体。

### 5.1 类定义

```cpp
class ReadTemplateHandler final : public backend_core::v1::domain::QueryHandler
```

含义：

1. 这是一个具体业务 handler
2. 它继承的是 `QueryHandler`
3. 所以它属于“读操作模板”

### 5.2 实现 ExecuteQuery

```cpp
backend_core::v1::TaskEnvelope ExecuteQuery(const backend_core::v1::TaskRequest& request) const override
```

含义：

1. 所有查询逻辑写在这里
2. 输入是统一的 `TaskRequest`
3. 输出是统一的 `TaskEnvelope`

当前示例里，它做了三件事：

1. 构造一个 `ContentItem`
2. 给字段填默认值或请求值
3. 返回 `BuildSuccessWithItemEnvelope`

也就是说，后续你写查询类业务，通常只需要把这里替换成真实查询逻辑。

### 5.3 入口桥接函数

```cpp
backend_core::v1::TaskEnvelope HandleReadTemplateEntry(const backend_core::v1::TaskRequest& request)
{
    return backend_core::v1::domain::InvokeFrameworkHandler<ReadTemplateHandler>(request);
}
```

这个函数的作用不是写业务，而是“桥接注册系统”。

职责只有一个：

把框架收到的请求，转发给 `ReadTemplateHandler`。

### 5.4 注册

```cpp
AUTO_REGISTER("template", "read", "get", HandleReadTemplateEntry);
```

含义：

1. 这个 handler 被注册到路由 `template/read/get`
2. HTTP 请求命中这个路由时，会进入 `HandleReadTemplateEntry`
3. 然后由它再调用真正类实现

## 6. 写模板逐步讲解

文件：`backend/domain/handlers/HealthPingHandler.cpp`

虽然文件名还是旧名字，但它现在实际上承担的是“写样板”的角色。

### 6.1 类定义

```cpp
class WriteTemplateHandler final : public backend_core::v1::domain::CommandHandler
```

含义：

1. 这是一个具体业务 handler
2. 它继承的是 `CommandHandler`
3. 所以它属于“写操作模板”

### 6.2 Validate

```cpp
std::optional<backend_core::v1::domain::DomainError> Validate(const backend_core::v1::TaskRequest& request) const override
```

职责：

1. 校验请求是否合法
2. 不合法就返回 `DomainError`
3. 合法就返回 `std::nullopt`

当前示例里只校验了一件最基础的事：

- `taskId` 不能为空

这是最小校验样板。后续真实业务里，你应该在这里补：

- 必填字段校验
- 格式校验
- 枚举值校验
- 业务前置条件校验

### 6.3 ExecuteCommand

```cpp
backend_core::v1::TaskEnvelope ExecuteCommand(const backend_core::v1::TaskRequest& request) const override
```

职责：

1. 执行真正写操作
2. 返回统一结果

当前示例里只是返回：

- `BuildSuccessEnvelope(request, "write_template_ok")`

这表示“写操作成功”的最小样板。

### 6.4 入口桥接函数

```cpp
backend_core::v1::TaskEnvelope HandleWriteTemplateEntry(const backend_core::v1::TaskRequest& request)
{
    return backend_core::v1::domain::InvokeFrameworkHandler<WriteTemplateHandler>(request);
}
```

它的职责和读模板一样：

1. 给 `AUTO_REGISTER` 用
2. 把请求转交给类实现

### 6.5 注册

```cpp
AUTO_REGISTER("template", "write", "upsert", HandleWriteTemplateEntry);
```

含义：

1. 这个 handler 被注册到 `template/write/upsert`
2. HTTP 请求命中该路由时，会执行写模板

## 7. 为什么必须有 `HandleWriteTemplateEntry` 这种函数

这是很多人第一次看模板时最容易疑惑的点。

原因很简单：

1. `AUTO_REGISTER` 宏最稳定的用法，是传一个普通函数名
2. 具体业务实现现在放在类里
3. 所以需要一个普通函数当“注册入口”
4. 这个普通函数再调用类对象

所以：

- `WriteTemplateHandler` / `ReadTemplateHandler` 是“业务实现体”
- `HandleWriteTemplateEntry` / `HandleReadTemplateEntry` 是“注册桥接函数”

它们不是重复代码，而是分工不同。

## 8. 实际复制粘贴替换步骤

后续开发者新增业务时，建议严格按下面步骤：

### 8.1 如果你要新增“读操作”

推荐直接复制：

- `backend/domain/handlers/UserProfileGetHandler.cpp`

然后依次替换：

1. 文件名
   例如改成 `OrderGetHandler.cpp`

2. 类名
   `ReadTemplateHandler` -> `OrderGetHandler`

3. 入口函数名
   `HandleReadTemplateEntry` -> `HandleOrderGetEntry`

4. 路由三段
   例如：
   - `template` -> `order`
   - `read` -> `detail`
   - `get` -> `get`

5. `ExecuteQuery` 里的内容
   - 把默认演示字段替换成真实业务查询结果

6. 如果有输入要求，可以额外覆盖 `Validate`

### 8.2 如果你要新增“写操作”

推荐直接复制：

- `backend/domain/handlers/HealthPingHandler.cpp`

然后依次替换：

1. 文件名
   例如改成 `OrderUpsertHandler.cpp`

2. 类名
   `WriteTemplateHandler` -> `OrderUpsertHandler`

3. 入口函数名
   `HandleWriteTemplateEntry` -> `HandleOrderUpsertEntry`

4. 路由三段
   例如：
   - `template` -> `order`
   - `write` -> `command`
   - `upsert` -> `upsert`

5. `Validate`
   - 校验业务必填字段

6. `ExecuteCommand`
   - 写真正的新增/修改/删除逻辑

7. 如果业务需要幂等或版本控制，再补：
   - `EnsureIdempotency`
   - `CheckVersionConflict`

## 9. 哪些地方应该改，哪些地方不要改

### 9.1 必须改的地方

1. 类名
2. 入口函数名
3. `AUTO_REGISTER` 里的三段路由
4. `Validate`
5. `ExecuteQuery` 或 `ExecuteCommand`

### 9.2 尽量不要改的地方

1. 匿名命名空间结构
2. `InvokeFrameworkHandler<YourHandler>(request)` 这一层桥接写法
3. `AUTO_REGISTER(..., HandleXXXEntry)` 这种注册模式
4. 统一 Envelope 构建函数调用模式

## 10. 设计这套最小完备基础函数操作时，要考虑哪些方面

建议至少从下面 6 个维度考虑：

### 10.1 输入校验

是否有统一校验入口？

当前答案：有，通过 `Validate()`。

### 10.2 读写分离

是否区分查询与命令？

当前答案：有，通过 `QueryHandler` 和 `CommandHandler`。

### 10.3 错误统一

是否所有错误都通过统一结构返回？

当前答案：有，通过 `DomainError` + `BuildErrorEnvelope()`。

### 10.4 返回统一

是否所有 handler 都返回相同格式？

当前答案：有，通过 `TaskEnvelope`。

### 10.5 可扩展性

后续是否容易增加幂等、并发控制、鉴权、审计？

当前答案：有基础挂点，尤其是命令侧已预留：

- `EnsureIdempotency()`
- `CheckVersionConflict()`

### 10.6 对新开发者是否友好

是否能做到“复制模板后只改少量点”？

当前答案：可以，核心只改：

1. 文件名
2. 类名
3. 入口函数名
4. 路由三段
5. 业务执行函数

## 11. 当前推荐的开发规范

为了保证后续业务代码风格统一，建议项目内固定以下规则：

1. 所有业务 handler 都放在 `backend/domain/handlers/`
2. 所有业务 handler 都 include `backend/core/api/BackendDev.h`
3. 读操作统一继承 `QueryHandler`
4. 写操作统一继承 `CommandHandler`
5. 校验统一写在 `Validate()`
6. 成功返回统一使用 `BuildSuccess*Envelope`
7. 失败返回统一使用 `DomainError` + `BuildErrorEnvelope`
8. 注册统一使用 `AUTO_REGISTER(..., HandleXXXEntry)`
9. `main.cpp` 不允许写业务 handler
10. 运行时与业务逻辑分离，业务只改 `domain/handlers/`

## 12. 当前最小验证链路

当前项目已经有最小 smoke test：

1. 运行时会注册所有 handler
2. smoke test 会调用路由 `template/read/get`
3. 命中读模板
4. 返回 `read_template_ok`

这说明：

1. 注册链路正常
2. 路由分发正常
3. 模板 handler 调用正常
4. 框架可作为后续业务开发底座继续扩展

## 13. 一句话总结

当前这套设计的核心思想是：

**把业务开发约束成“读模板”和“写模板”两种固定形态，让后续开发者只改业务规则，不改框架入口。**
