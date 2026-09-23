//
// Created by new on 2026/9/18.
//

#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <string_view>
#include <array>
#include <cassert>
#include <thread>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <utility>
#include <source_location>

namespace halolog
{
    enum class LogLevel : int
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    constexpr std::string_view levelToString(LogLevel level)
    {
        assert(static_cast<int>(level) >= 0 && static_cast<int>(level) < 6);
        constexpr std::array<std::string_view, 6> names = {
            "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
        return names[static_cast<int>(level)];
    }

    constexpr std::string_view levelToShortString(LogLevel level)
    {
        assert(static_cast<int>(level) >= 0 && static_cast<int>(level) < 6);
        constexpr std::array<std::string_view, 6> names = {
            "T", "D", "I", "W", "E", "F"};
        return names[static_cast<int>(level)];
    }

    class Record
    {
    public:
        Record(LogLevel lev, const std::string &msg = {""},
               const std::source_location &location = std::source_location::current())
            : Record(lev, msg, std::chrono::system_clock::now(),
                     std::this_thread::get_id(),
                     location.file_name(), std::filesystem::path(location.file_name()).filename().string(),
                     location.function_name(),
                     location.line()) {}

        Record(LogLevel lev, const std::string &msg,
               std::string fullfile,
               std::string filename,
               std::string function,
               std::uint_least32_t line)
            : Record(lev, msg, std::chrono::system_clock::now(),
                     std::this_thread::get_id(), fullfile, filename, function, line) {}

        Record(LogLevel lev, const std::string &msg,
               std::chrono::system_clock::time_point timestamp,
               std::thread::id threadid,
               std::string fullfile,
               std::string filename,
               std::string fun,
               std::uint_least32_t line)
            : level(lev), timestamp(timestamp), threadId(threadid),
              fullFile(fullfile), fileName(filename), function(fun), line(line)
        {
            message << msg;
        }

        template <typename T>
        Record &operator<<(T &&value)
        {
            message << std::forward<T>(value);
            return *this;
        }

        Record &operator<<(std::ostream &(*manipulator)(std::ostream &))
        {
            manipulator(message);
            return *this;
        }

        Record &operator<<(std::ios_base &(*manipulator)(std::ios_base &))
        {
            manipulator(message);
            return *this;
        }

        LogLevel level;
        std::ostringstream message;

        std::chrono::system_clock::time_point timestamp;
        std::thread::id threadId;
        std::string fullFile;
        std::string fileName;
        std::string function;
        std::uint_least32_t line;
    };

}

#endif // RECORD_H
