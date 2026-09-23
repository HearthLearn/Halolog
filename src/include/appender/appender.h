//
// Created by new on 2026/9/18.
//

#ifndef APPENDER_H
#define APPENDER_H
#include "record.h"
#include "formatter.h"
#include <memory>

namespace halolog
{
    class Appender
    {
    public:
        virtual ~Appender() = default;
        virtual bool doAppend(const Record &record) = 0;

        bool append(const Record &record)
        {
            if (record.level >= mMinLevel)
            {
                return doAppend(record);
            }
            return true;
        }

        void setFormatter(std::unique_ptr<Formatter> formatter)
        {
            if (formatter)
                mFormatter = std::move(formatter);
        }

        const Formatter &formatter() const { return *mFormatter; }

        void setMinLevel(LogLevel level)
        {
            mMinLevel = level;
        }

        const LogLevel minLevel() { return mMinLevel; }

    private:
        LogLevel mMinLevel{LogLevel::Trace};
        std::unique_ptr<Formatter> mFormatter{std::make_unique<Formatter>("{d:%Y-%m-%d %H:%M:%S} [{P}] {m}{n}")};
    };

}

#endif // APPENDER_H
