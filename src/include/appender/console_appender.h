//
// Created by new on 2026/9/18.
//

#ifndef CONSOLE_APPENDER_H
#define CONSOLE_APPENDER_H
#include "stream_appender.h"
#include "utils.h"

namespace halolog
{
    class ConsoleAppender : public StreamAppender
    {
    public:
        ConsoleAppender(std::ostream &stream = std::cout,
                        bool flush = true,
                        bool color = true)
            : StreamAppender(stream, flush), mColor{color && ttyColorSupport(stream)}
        {
        }
        ~ConsoleAppender() override = default;

    protected:
        std::string_view prefix(const Record &record) override
        {
            if (!mColor)
            {
                return "";
            }
            switch (record.level)
            {
            case LogLevel::Trace:
                return "\033[90m";
            case LogLevel::Debug:
                return "\033[36m";
            case LogLevel::Info:
                return "\033[32m";
            case LogLevel::Warning:
                return "\033[33m";
            case LogLevel::Error:
                return "\033[31m";
            case LogLevel::Fatal:
                return "\033[35;1m";
            default:
                return "";
            }
        }
        std::string_view suffix(const Record &record) override
        {
            return mColor ? "\033[0m" : "";
        }

    private:
        bool mColor{false};
    };
}

#endif // CONSOLE_APPENDER_H