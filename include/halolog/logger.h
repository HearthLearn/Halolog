//
// Created by new on 2026/9/18.
//

#ifndef HALOLOG_LOGGER_H
#define HALOLOG_LOGGER_H

#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <halolog/appender/appender.h>
#include <halolog/record.h>

namespace halolog
{
    class Logger
    {
    private:
        static auto &pool()
        {
            static std::unordered_map<std::string, std::unique_ptr<Logger>> p;
            static std::once_flag flag;
            std::call_once(flag, [&]
                           { p.emplace("def", std::unique_ptr<Logger>(new Logger("def"))); });
            return p;
        }

    public:
        static Logger &registry(const std::string &name)
        {
            auto [it, ok] = pool().try_emplace(name, std::unique_ptr<Logger>(new Logger(name)));
            if (!ok)
            {
                throw std::invalid_argument("Logger '" + name + "' already registered");
            }
            return *it->second;
        }

        static Logger &get(const std::string &name = "def")
        {
            auto &p = pool();
            auto it = p.find(name);
            if (it == p.end())
            {
                throw std::invalid_argument("Logger '" + name + "' not registered");
            }
            return *it->second;
        }

        const std::string &name() const
        {
            return mName;
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

        void flush()
        {
            for (const auto &appender : mAppenders)
            {
                if (appender)
                {
                    appender->flush();
                }
            }
        }

    private:
        explicit Logger(std::string name) : mName(std::move(name)) {};
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;
        Logger(Logger &&) = delete;
        Logger &operator=(Logger &&) = delete;

        std::string mName;
        LogLevel mMinLevel{LogLevel::Trace};
        std::vector<std::shared_ptr<Appender>> mAppenders;
    };

};

#endif // HALOLOG_LOGGER_H
