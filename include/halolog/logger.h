//
// Created by new on 2026/9/18.
//

#ifndef HALOLOG_LOGGER_H
#define HALOLOG_LOGGER_H
#include <memory>
#include <halolog/appender/appender.h>
#include <vector>
#include <halolog/record.h>
#include <format>
namespace halolog
{
    class Logger
    {
    public:
        static Logger &instance()
        {
            static Logger logger;
            return logger;
        }

        void addAppender(std::shared_ptr<Appender> appender)
        {
            mAppenders.push_back(std::move(appender));
        }

        void setMinLevel(LogLevel level)
        {
            mMinLevel = level;
        }

        LogLevel minLevel() const
        {
            return mMinLevel;
        }

        void operator+=(const Record &record)
        {
            for (const auto &appender : mAppenders)
            {
                if (appender)
                {
                    appender->append(record);
                }
            }
        }

    private:
        Logger() {};
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;
        Logger(Logger &&) = delete;
        Logger &operator=(Logger &&) = delete;

        LogLevel mMinLevel{LogLevel::Trace};
        std::vector<std::shared_ptr<Appender>> mAppenders;
    };

};

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

#endif // HALOLOG_LOGGER_H
