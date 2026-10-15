//
// Created by new on 2026/9/30.
//

#ifndef HALOLOG_H
#define HALOLOG_H

#include <format>
#include <halolog/logger.h>

namespace halolog
{
    inline void log(const Record &record, const std::string &name = "def")
    {
        auto &logger = Logger::get(name);
        if (record.level >= logger.minLevel())
        {
            logger += std::move(record);
        }
    }

#define HLOG(logger, level, ...)   \
    if (level < logger.minLevel()) \
    {                              \
        ;                          \
    }                              \
    else                           \
        logger += halolog::Record(level __VA_OPT__(, std::format(__VA_ARGS__)))

#define HLOG_LOG(name, level, ...) HLOG(halolog::Logger::get(name), level __VA_OPT__(, ) __VA_ARGS__)

#define HLOG_T(...) HLOG_LOG("def", halolog::LogLevel::Trace __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_D(...) HLOG_LOG("def", halolog::LogLevel::Debug __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_I(...) HLOG_LOG("def", halolog::LogLevel::Info __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_W(...) HLOG_LOG("def", halolog::LogLevel::Warning __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_E(...) HLOG_LOG("def", halolog::LogLevel::Error __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_F(...) HLOG_LOG("def", halolog::LogLevel::Fatal __VA_OPT__(, ) __VA_ARGS__)

#define HLOG_LOGGER_T(name, ...) HLOG_LOG(name, halolog::LogLevel::Trace __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_LOGGER_D(name, ...) HLOG_LOG(name, halolog::LogLevel::Debug __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_LOGGER_I(name, ...) HLOG_LOG(name, halolog::LogLevel::Info __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_LOGGER_W(name, ...) HLOG_LOG(name, halolog::LogLevel::Warning __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_LOGGER_E(name, ...) HLOG_LOG(name, halolog::LogLevel::Error __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_LOGGER_F(name, ...) HLOG_LOG(name, halolog::LogLevel::Fatal __VA_OPT__(, ) __VA_ARGS__)
}

#endif // HALOLOG_H
