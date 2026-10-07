# Halolog

轻量级 C++20 header-only 日志库，基于 `std::format` 和 `std::source_location`。

- **Header-only**：无需编译，引入即用
- **双模式写入**：`std::format` 格式化 + 流式 `operator<<`，可混合使用
- **可定制格式**：类 log4j 模式字符串，支持 `std::format` 格式规格
- **多种输出**：控制台（自动彩色）、时间滚动文件
- **级别过滤**：Logger 和 Appender 均可独立设置最低级别

## 环境要求
- C++20
- CMake ≥ 3.23

## 快速开始

```cpp
#include <halolog/halolog.h>
#include <halolog/appender/console_appender.h>

int main() {
    auto& logger = halolog::Logger::instance();
    logger.addAppender(std::make_shared<halolog::ConsoleAppender>());

    HLOG_I("hello {}", "halolog");       // std::format 风格
    HLOG_W() << "value=" << 42;          // 流式风格
    HLOG_D("id={}", 1001) << ", extra"; // 混合风格

    return 0;
}
```

输出示例：

```
2026-09-21 14:30:00.123 [I] hello halolog
2026-09-21 14:30:00.124 [W] value=42
2026-09-21 14:30:00.124 [D] id=1001, extra
```

## 集成方式
### add_subdirectory

将 halolog 作为子目录（或 git submodule）添加到项目中：

```cmake
add_subdirectory(3rd/halolog EXCLUDE_FROM_ALL)
target_link_libraries(your_target PRIVATE halolog::halolog)
```

### find_package

```cmake
find_package(halolog CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE halolog::halolog)
```

## 使用指南

> **注意：** 所有配置（`setMinLevel`、`setFormatter`、`addAppender`）等必须在开始日志记录**之前**完成。运行时修改配置是**非线程安全**的。

### 日志级别

从低到高共 6 个级别，每个级别有对应的宏：

| 级别    | 宏            | 说明     |
| ------- | ------------- | -------- |
| Trace   | `HLOG_T(...)` | 详细追踪 |
| Debug   | `HLOG_D(...)` | 调试信息 |
| Info    | `HLOG_I(...)` | 一般信息 |
| Warning | `HLOG_W(...)` | 警告     |
| Error   | `HLOG_E(...)` | 错误     |
| Fatal   | `HLOG_F(...)` | 致命错误 |

Logger 和每个 Appender 都可以独立设置最低级别：

```cpp
logger.setMinLevel(halolog::LogLevel::Info);      // Logger 级别过滤
console->setMinLevel(halolog::LogLevel::Warning); // Appender 级别过滤
```

### Appender

**ConsoleAppender** — 控制台输出，自动检测终端彩色支持：

```cpp
// 默认输出到 stdout，启用颜色和自动 flush
auto console = std::make_shared<halolog::ConsoleAppender>();

// 输出到 stderr，禁用颜色
auto console = std::make_shared<halolog::ConsoleAppender>(std::cerr, false);
```

**RollingFileAppender** — 时间滚动文件，路径支持 `std::chrono` 时间格式：

```cpp
// logs/app_2026-09-21.log
auto rolling = std::make_shared<halolog::RollingFileAppender>("logs/app_{:%Y-%m-%d}.log");

// logs/2026/09/21/14-30-00.log
auto rolling = std::make_shared<halolog::RollingFileAppender>("logs/{:%Y}/{:%m}/{:%d}/{:%H-%M-%S}.log");

// logs/2026-09-21/app.log
auto rolling = std::make_shared<halolog::RollingFileAppender>("logs/{:%Y-%m-%d}/app.log");
```

### Formatter 模式字符串

每个 Appender 可设置独立的格式化模式。默认模式为 `{d:%Y-%m-%d %H:%M:%S} [{P}] {m}{n}`。

可用字段：

| 字段   | 含义             | 示例输出                  |
| ------ | ---------------- | ------------------------- |
| `{m}`  | 消息正文         | `hello halolog`           |
| `{p}`  | 级别全名（大写） | `WARNING`                 |
| `{P}`  | 级别缩写         | `W`                       |
| `{t}`  | 线程 ID          | `40616`                   |
| `{d}`  | 时间戳           | `2026-09-21 08:01:55.748` |
| `{F}`  | 源文件名         | `main.cpp`                |
| `{fp}` | 源文件完整路径   | `xxx/src/main.cpp`        |
| `{L}`  | 行号             | `42`                      |
| `{M}`  | 函数名           | `int __cdecl main(void)`  |
| `{n}`  | 换行符           | —                         |

字段支持 `std::format` 格式规格（冒号后追加）：

```cpp
auto fmt = std::make_unique<halolog::Formatter>(
    "{d:%H:%M:%S} {p:<7} {F}:{L:04} {m}{n}"
);
console->setFormatter(std::move(fmt));
// 输出: 14:30:00 INFO    main.cpp:0042 hello halolog
```

字面量 `{` 和 `}` 通过 `{{` / `}}` 转义。

## 项目结构

```
halolog/
├── include/
│   └── halolog/
│       ├── halolog.h                  # 日志宏
│       ├── logger.h                   # Logger 单例
│       ├── record.h                   # 日志记录
│       ├── formatter.h                # 模式字符串解析与格式化
│       ├── utils.h                    # 工具类
│       └── appender/
│           ├── appender.h             # Appender 基类
│           ├── console_appender.h     # 支持颜色的控制台附加器
│           ├── file_appender.h        # 文件基类
│           └── rollingfile_appender.h # 时间滚动文件附加器
├── example/
│   ├── CMakeLists.txt                 # 示例项目入口
│   ├── integration/                   # 集成方式示例（独立配置和构建）
│   │   ├── add_subdirectory/
│   │   └── find_package/
│   └── usage/                         # 功能使用示例
│       ├── console/                   # 控制台日志示例
│       ├── rolling_file/              # 时间滚动文件日志示例
│       └── qt/                        # Qt 集成示例
├── cmake/                             # CMake 配置模板
└── CMakeLists.txt
```

## License

MIT
