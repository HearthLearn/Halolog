//
// Created by new on 2026/9/18.
//
#include "logger.h"
#include "appender/console_appender.h"

int main()
{
    auto &logger = halolog::Logger::instance();
    auto console = std::make_shared<halolog::ConsoleAppender>();
    logger.addAppender(console);

    HLOG_D("sample test");
    HLOG_D("first = ") << std::string("1") << "2";

    return 0;
}
