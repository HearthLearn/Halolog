//
// Created by new on 2026/9/18.
//

#ifndef CONSOLE_APPENDER_H
#define CONSOLE_APPENDER_H
#include "appender.h"
#include "utils.h"

namespace halolog
{
    class ConsoleAppender : public Appender
    {
    public:
        ConsoleAppender(std::ostream &stream = std::cout,
                        bool color = true,
                        bool flush = true)
            : mStream{stream},
              mColor{color && ttyColorSupport(stream)},
              mFlush{flush}
        {
        }
        ~ConsoleAppender() override = default;

        bool doAppend(const Record &record) override
        {
            const std::string message = record.message.str();

            std::string buff;
            if (mColor)
            {
                const std::string_view begin = prefix(record.level);
                const std::string_view end = suffix();
                buff.reserve(message.size() + begin.size() + end.size() + 1);
                buff.append(begin);
                buff.append(message);
                buff.append(end);
            }
            else
            {
                buff.reserve(message.size() + 1);
                buff.append(message);
            }

            buff.push_back('\n');

            mStream.write(
                buff.data(),
                static_cast<std::streamsize>(buff.size()));
            if (mFlush)
            {
                mStream.flush();
            }
            return mStream.good();
        }

    private:
        std::string_view prefix(const LogLevel &level)
        {
            switch (level)
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
        std::string_view suffix()
        {
            return "\033[0m";
        }

    private:
        std::ostream &mStream;
        bool mColor{false};
        bool mFlush{false};
    };
}

#endif // CONSOLE_APPENDER_H