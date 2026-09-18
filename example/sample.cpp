//
// Created by new on 2026/9/18.
//
#include "logger.h"
#include "appender/console_appender.h"

int main()
{
    using namespace halolog;
    auto &logger = Logger::instance();
    auto console = std::make_shared<ConsoleAppender>();
    logger.addAppender(console);

    LOG_DEBUG("test");

    return 0;
}
