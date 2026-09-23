//
// Created by Administrator on 2026/9/21.
//

#ifndef HALOLOG_DAILYFILE_APPENDER_H
#define HALOLOG_DAILYFILE_APPENDER_H

#include "file_appender.h"
#include <chrono>
#include <format>
#include <stdexcept>

namespace halolog
{
    class DailyFileAppender : public FileAppender
    {
    public:
        explicit DailyFileAppender(const std::string &pattern, bool flush = true)
            : mPattern(pattern),
              mFlush(flush)
        {
            validatePattern();
        };

        bool doAppend(const Record &record) override
        {
            auto newpath = rotate(record.timestamp);
            if (newpath != mCurrentPath)
            {
                close();
                if (!checkPath(newpath))
                {
                    return false;
                }
                open(newpath);
                if (!isOpen())
                {
                    return false;
                }
                mCurrentPath = newpath;
            }
            std::string message = formatter().format(record);
            bool result = write(message);
            if (mFlush)
            {
                flush();
            }
            return result;
        }

    private:
        void validatePattern() const
        {
            try
            {
                auto date = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
                static_cast<void>(std::vformat(mPattern, std::make_format_args(date)));
            }
            catch (const std::format_error &e)
            {
                throw std::invalid_argument("Invalid pattern: " + mPattern + ". Error: " + e.what());
            }
        }

        std::filesystem::path rotate(
            std::chrono::system_clock::time_point tp) const
        {
            auto local = std::chrono::current_zone()->to_local(tp);
            auto str = std::vformat(mPattern, std::make_format_args(local));
            return std::filesystem::path(str).lexically_normal();
        }

        std::string mPattern;
        bool mFlush{true};
        std::filesystem::path mCurrentPath;
    };
}

#endif // HALOLOG_DAILYFILE_APPENDER_H
