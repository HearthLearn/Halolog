#pragma once

#include <catch2/catch_all.hpp>

#include <halolog/halolog.h>
#include <halolog/appender/appender.h>
#include <halolog/appender/console_appender.h>
#include <halolog/appender/file_appender.h>
#include <halolog/appender/rollingfile_appender.h>

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <stdexcept>
#include <filesystem>

using enum halolog::LogLevel;

namespace fs = std::filesystem;

// ── Test-only appender: captures messages in memory ──────────────
class TestAppender : public halolog::Appender
{
public:
    explicit TestAppender(halolog::FlushConfig flush = halolog::FlushConfig::manual())
    {
        setFormatter(std::make_unique<halolog::Formatter>("{m}"));
        setFlush(flush);
    }

    bool doAppend(const halolog::Record &record) override
    {
        std::string msg = formatter().format(record);
        {
            std::lock_guard<std::mutex> lock(mMsgMutex);
            mMessages.push_back(std::move(msg));
        }
        mMsgCount.fetch_add(1, std::memory_order_relaxed);
        return true;
    }

    void doFlush() override
    {
        mFlushCount.fetch_add(1, std::memory_order_relaxed);
    }

    int msgCount() const { return mMsgCount.load(std::memory_order_relaxed); }
    int flushCount() const { return mFlushCount.load(std::memory_order_relaxed); }

    std::vector<std::string> messages() const
    {
        std::lock_guard<std::mutex> lock(mMsgMutex);
        return mMessages;
    }

private:
    std::atomic<int> mMsgCount{0};
    std::atomic<int> mFlushCount{0};
    mutable std::mutex mMsgMutex;
    std::vector<std::string> mMessages;
};

// ── Helper: read non-empty lines from a file ────────────────────
inline std::vector<std::string> readLines(const fs::path &path)
{
    std::vector<std::string> lines;
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("Failed to open test log file: " + path.string());
    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty())
            lines.push_back(std::move(line));
    }
    return lines;
}

// ── Helper: generate a unique logger name per test case ─────────
inline std::string uniqueName(const std::string &prefix)
{
    static std::atomic<int> counter{0};
    return prefix + "_" + std::to_string(counter.fetch_add(1));
}

// ── Helper: create a Record with a specific timestamp ──────────
inline halolog::Record makeTestRecord(
    halolog::LogLevel level = Info,
    const std::string &loggerName = "test",
    const std::string &message = "hello",
    std::chrono::system_clock::time_point ts = std::chrono::system_clock::now())
{
    return halolog::Record(level, loggerName, message, ts,
                           std::this_thread::get_id(),
                           "test.cpp", "test.cpp", "testFunc", 1);
}
