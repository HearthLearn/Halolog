//
// Created by new on 2026/9/18.
//

#ifndef CONSOLE_APPENDER_H
#define CONSOLE_APPENDER_H
#include "appender.h"
#include <iostream>

namespace halolog
{
    class ConsoleAppender : public Appender
    {
    public:
        ConsoleAppender() = default;
        ~ConsoleAppender() override = default;

        bool doAppend(const Record &record) override
        {
            std::cout << record.message.str() << std::endl;
            return true;
        }
    };

}

#endif // CONSOLE_APPENDER_H
