//
// Created by new on 2026/9/18.
//

#ifndef HALOLOG_LOGGER_H
#define HALOLOG_LOGGER_H

#include <memory>
#include <vector>
#include <halolog/appender/appender.h>
#include <halolog/record.h>

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

#endif // HALOLOG_LOGGER_H
