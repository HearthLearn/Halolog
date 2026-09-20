//
// Created by new on 2026/9/18.
//

#ifndef LOGGER_H
#define LOGGER_H
#include <memory>
#include "appender/appender.h"
#include <vector>
#include "record.h"
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

#endif // LOGGER_H
