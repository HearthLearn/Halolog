//
// Created by Administrator on 2026/9/20.
//

#ifndef HALOLOG_STREAM_APPENDER_H
#define HALOLOG_STREAM_APPENDER_H
#include "appender.h"
#include <iostream>
#include <string>
#include <string_view>

namespace halolog
{
    class StreamAppender : public Appender
    {
    public:
        StreamAppender(std::ostream &stream, bool flush = false) : mStream(&stream), mFlush(flush) {}
        virtual ~StreamAppender() = default;

        bool doAppend(const Record &record) override
        {
            if (mStream)
            {
                const std::string message = record.message.str();
                const std::string_view begin = prefix(record);
                const std::string_view end = suffix(record);

                std::string buff;
                buff.reserve(begin.size() + message.size() + end.size());
                buff.append(begin);
                buff.append(message);
                buff.append(end);

                buff.push_back('\n');

                mStream->write(
                    buff.data(),
                    static_cast<std::streamsize>(buff.size()));
                if (mFlush)
                {
                    mStream->flush();
                }
                return mStream->good();
            }
            return false;
        }

    protected:
        virtual std::string_view prefix(const Record &record) { return {}; }
        virtual std::string_view suffix(const Record &record) { return {}; }

    private:
        std::ostream *mStream{nullptr};
        bool mFlush{false};
    };

} // namespace halolog

#endif // HALOLOG_STREAM_APPENDER_H
