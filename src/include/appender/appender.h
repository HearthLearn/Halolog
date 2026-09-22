//
// Created by new on 2026/9/18.
//

#ifndef APPENDER_H
#define APPENDER_H
#include "record.h"

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

        void setMinLevel(LogLevel level)
        {
            mMinLevel = level;
        }

        const LogLevel minLevel() { return mMinLevel; }

    private:
        LogLevel mMinLevel{LogLevel::Trace};
    };

}

#endif // APPENDER_H
