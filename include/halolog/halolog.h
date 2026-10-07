//
// Created by new on 2026/9/30.
//

#ifndef HALOLOG_H
#define HALOLOG_H

#include <format>
#include <halolog/logger.h>

namespace halolog
{

#define HLOG(logger, level, ...)   \
    if (level < logger.minLevel()) \
    {                              \
        ;                          \
    }                              \
    else                           \
        logger += halolog::Record(level __VA_OPT__(, std::format(__VA_ARGS__)))

#define DEF_HLOG(level, ...) HLOG(halolog::Logger::instance(), level __VA_OPT__(, ) __VA_ARGS__)

#define HLOG_T(...) DEF_HLOG(halolog::LogLevel::Trace __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_D(...) DEF_HLOG(halolog::LogLevel::Debug __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_I(...) DEF_HLOG(halolog::LogLevel::Info __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_W(...) DEF_HLOG(halolog::LogLevel::Warning __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_E(...) DEF_HLOG(halolog::LogLevel::Error __VA_OPT__(, ) __VA_ARGS__)
#define HLOG_F(...) DEF_HLOG(halolog::LogLevel::Fatal __VA_OPT__(, ) __VA_ARGS__)

#define HLOG_APPEND_TO(logger, level, record)   \
    do                                          \
    {                                           \
        auto &&hlogLogger = (logger);           \
        const auto hlogLevel = (level);         \
        if (hlogLevel >= hlogLogger.minLevel()) \
        {                                       \
            hlogLogger += (record);             \
        }                                       \
    } while (false)

#define HLOG_APPEND(level, record) \
    HLOG_APPEND_TO(halolog::Logger::instance(), level, record)
}

#endif // HALOLOG_H
