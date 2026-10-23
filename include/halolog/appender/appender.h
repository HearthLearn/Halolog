//
// Created by new on 2026/9/18.
//

#ifndef HALOLOG_APPENDER_H
#define HALOLOG_APPENDER_H
#include <halolog/record.h>
#include <halolog/formatter.h>
#include <memory>
#include <mutex>

namespace halolog
{
    enum class FlushPolicy
    {
        Every,
        OnLevel,
        Count,
        Manual
    };

    struct FlushConfig
    {
        FlushPolicy policy{FlushPolicy::Every};
        LogLevel level{LogLevel::Error};
        int count{1};

        static FlushConfig every() { return {.policy = FlushPolicy::Every}; }
        static FlushConfig onLevel(LogLevel l = LogLevel::Error) { return {.policy = FlushPolicy::OnLevel, .level = l}; }
        static FlushConfig onCount(int n) { return {.policy = FlushPolicy::Count, .count = n}; };
        static FlushConfig manual() { return {.policy = FlushPolicy::Manual}; }
    };

    class Appender
    {
    public:
        virtual ~Appender() = default;
        virtual bool doAppend(const Record &record) = 0;
        virtual void doFlush() {}

        bool append(const Record &record)
        {
            if (record.level >= mMinLevel)
            {
                std::lock_guard<std::mutex> lock(mMutex);
                bool ok = doAppend(record);
                if (ok)
                {
                    autoFlush(record);
                }
                return ok;
            }
            return true;
        }

        void flush()
        {
            std::lock_guard<std::mutex> lock(mMutex);
            doFlush();
            mFlushCounter = 0;
        }

        void setFlush(const FlushConfig &config)
        {
            mFlushPolicy = config.policy;
            mFlushLevel = config.level;
            mFlushCount = config.count > 0 ? config.count : 1;
            mFlushCounter = 0;
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
        void autoFlush(const Record &record)
        {
            switch (mFlushPolicy)
            {
            case FlushPolicy::Every:
            {
                doFlush();
                break;
            }
            case FlushPolicy::OnLevel:
            {
                if (record.level >= mFlushLevel)
                {
                    doFlush();
                }
                break;
            }
            case FlushPolicy::Count:
            {
                if (++mFlushCounter >= mFlushCount)
                {
                    doFlush();
                    mFlushCounter = 0;
                }
                break;
            }
            case FlushPolicy::Manual:
            {
                break;
            }
            }
        }

        std::mutex mMutex;
        LogLevel mMinLevel{LogLevel::Trace};
        std::unique_ptr<Formatter> mFormatter{std::make_unique<Formatter>("{d:%Y-%m-%d %H:%M:%S} [{P}] {m}{n}")};

        FlushPolicy mFlushPolicy{FlushPolicy::Every};
        LogLevel mFlushLevel{LogLevel::Error};
        int mFlushCount{1};
        int mFlushCounter{0};
    };
}

#endif // HALOLOG_APPENDER_H
