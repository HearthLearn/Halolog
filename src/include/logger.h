//
// Created by new on 2026/9/18.
//

#ifndef LOGGER_H
#define LOGGER_H
#include <memory>
#include "appender/appender.h"
#include <vector>
#include "record.h"

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

        const auto &appenders()
        {
            return mAppenders;
        }

    private:
        Logger() {};
        std::vector<std::shared_ptr<Appender>> mAppenders;
    };

};

#define HLOG(level, message)                                           \
    do                                                                 \
    {                                                                  \
        auto logger = halolog::Logger::instance();                     \
        {                                                              \
            for (const auto &appender : logger.appenders())            \
            {                                                          \
                if (level < appender->minLevel())                      \
                {                                                      \
                    ;                                                  \
                }                                                      \
                else                                                   \
                {                                                      \
                    appender->append(halolog::Record(level, message)); \
                }                                                      \
            }                                                          \
        }                                                              \
    } while (false);

#define LOG_TRACE(message) HLOG(halolog::LogLevel::Trace, message)
#define LOG_DEBUG(message) HLOG(halolog::LogLevel::Debug, message)
#define LOG_INFO(message) HLOG(halolog::LogLevel::Info, message)
#define LOG_WARNING(message) HLOG(halolog::LogLevel::Warning, message)
#define LOG_ERROR(message) HLOG(halolog::LogLevel::Error, message)
#define LOG_FATAL(message) HLOG(halolog::LogLevel::Fatal, message)

#endif // LOGGER_H
