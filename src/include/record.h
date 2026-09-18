//
// Created by new on 2026/9/18.
//

#ifndef RECORD_H
#define RECORD_H

#include <string>

namespace halolog
{
    enum class LogLevel : int
    {
        Trace,
        Debug,
        Info,
        Warning,
        ERROR,
        Fatal
    };

    class Record
    {
    public:
        Record(LogLevel lev, const std::string &msg)
        {
            level = lev;
            message = msg;
        }
        LogLevel level;
        std::string message;
    };
}

#endif // RECORD_H
