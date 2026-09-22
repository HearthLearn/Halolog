//
// Created by Administrator on 2026/9/21.
//
#include "appender/dailyfile_appender.h"
#include "logger.h"

int main()
{
    auto &logger = halolog::Logger::instance();

    // logs/app_2026-09-21.log
    auto daily = std::make_shared<halolog::DailyFileAppender>("logs/app_{:%Y-%m-%d}.log");
    logger.addAppender(daily);

    // logs/2026/09/21/14-30-00.log
    // auto daily = std::make_shared<halolog::DailyFileAppender>("logs/{:%Y}/{:%m}/{:%d}/{:%H-%M-%S}.log");

    // logs/2026-09-21/app.log
    // auto daily = std::make_shared<halolog::DailyFileAppender>("logs/{:%Y-%m-%d}/app.log");

    // ../output/logs/app_2026-09-21.log
    // auto daily = std::make_shared<halolog::DailyFileAppender>("../output/logs/app_{:%Y-%m-%d}.log");

    // C:/logs/app_2026-09-21.log
    // auto daily = std::make_shared<halolog::DailyFileAppender>("C:/logs/app_{:%Y-%m-%d}.log");

    HLOG_I("This is an info message.");

    return 0;
}
