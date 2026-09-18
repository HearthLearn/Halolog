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

        void append(const Record &record)
        {
            for (const auto &appender : mAppenders)
            {
                appender->append(record);
            }
        }

    private:
        Logger() {};
        std::vector<std::shared_ptr<Appender>> mAppenders;
    };

};

#define LOG(level, message)                                \
    do                                                     \
    {                                                      \
        Logger::instance().append(Record(level, message)); \
    } while (false);

#define LOG_TRACE(message) LOG(LogLevel::Trace, message)
#define LOG_DEBUG(message) LOG(LogLevel::Debug, message)
#define LOG_INFO(message) LOG(LogLevel::Info, message)
#define LOG_WARNING(message) LOG(LogLevel::Warning, message)
#define LOG_ERROR(message) LOG(LogLevel::Error, message)
#define LOG_FATAL(message) LOG(LogLevel::Fatal, message)

#endif // LOGGER_H
