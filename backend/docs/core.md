函数命名规范

A. 访问器与修改器 (JavaBeans 规范)
get...(): 获取值。

set...(): 设置值。

is...() / has...(): 返回 boolean 值。

B. 转换与工厂 (Creation & Conversion)
to...(): 类型转换。如 toString(), toDto()。

as...(): 视图转换（通常不产生深拷贝）。如 Arrays.asList()。

of() / from(): 静态工厂方法，用于创建实例。如 LocalDate.of(...), Stream.from(...)。

<!-- ??? -->
\. 状态切换与生命周期管理（State & Lifecycle）
publish / subscribe: 用于消息队列、事件驱动架构（如 publishEvent, subscribeChannel）。

register / unregister: 注册与注销监听器或组件（如 registerListener）。

activate / deactivate: 激活或停用（通常针对账户、策略、缓存）。

enable / disable: 启用或禁用某些功能开关。

B. 数据加工与比对（Processing & Comparison）
merge: 合并。通常指将两个对象或两个集合的属性合并为一个（如 mergeConfig）。

aggregate: 聚合。统计、求和或汇总一组数据（如 aggregateMonthlySales）。

compare: 比对。接收两个参数，返回它们之间的差异或大小关系。

diff: 差异比对。通常返回一个包含了具体哪些字段被修改了的差异报告对象。

C. 条件与拦截（Interception & Matching）
supports: 支持判断。常用于策略模式，判断当前类是否能处理某个参数（如 Spring Security 的 supports(Class<?> authentication)）。

intercept / filter: 拦截或过滤。

matches: 正则或条件匹配。

## 开发目标

完成核心库

使用源码链接

## 开发进度

### OK

- 完成cmakelist的自动完成

### TODO