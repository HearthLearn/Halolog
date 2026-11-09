//
// Created by new on 2026/9/21.
//

#ifndef HALOLOG_FILE_APPENDER_H
#define HALOLOG_FILE_APPENDER_H

#include <halolog/appender/appender.h>
#include <fstream>
#include <string>
#include <filesystem>

namespace halolog
{
    class FileAppender : public Appender
    {
    public:
        FileAppender() = default;
        ~FileAppender() override
        {
            close();
        }
        FileAppender(const FileAppender &) = delete;
        FileAppender &operator=(const FileAppender &) = delete;
        FileAppender(FileAppender &&) = delete;
        FileAppender &operator=(FileAppender &&) = delete;

        void doFlush() override { flushStream(); }

        void close()
        {
            if (isOpen())
            {
                flushStream();
                mFileStream.close();
            }
        }

    protected:
        bool checkPath(const std::filesystem::path &filepath)
        {
            std::error_code ec;
            auto parent_path = filepath.parent_path();
            if (!parent_path.empty() && !std::filesystem::exists(parent_path, ec))
            {
                if (ec)
                    return false;
                std::filesystem::create_directories(parent_path, ec);
                return !ec;
            }
            return true;
        }
        void open(const std::filesystem::path &filepath)
        {
            mFileStream = std::ofstream(filepath, std::ios::out | std::ios::app);
        }

        bool isOpen()
        {
            return mFileStream.is_open();
        }

        bool write(const std::string &txt)
        {
            if (!isOpen())
            {
                return false;
            }
            mFileStream.write(txt.data(), static_cast<std::streamsize>(txt.size()));
            return mFileStream.good();
        }

        void flushStream() { mFileStream.flush(); }

    private:
        std::ofstream mFileStream;
    };
}

#endif // HALOLOG_FILE_APPENDER_H
