#include <halolog/appender/rollingfile_appender.h>
#include <halolog/logger.h>

int main()
{
    auto &logger = halolog::Logger::instance();

    auto rolling = std::make_shared<halolog::RollingFileAppender>(
        "logs/app_{:%Y-%m-%d}.log");
    rolling->setMinLevel(halolog::LogLevel::Info);
    logger.addAppender(rolling);

    HLOG_D("This debug message is filtered by the appender.");
    HLOG_I("Rolling log file initialized.");
    HLOG_W("The same file is reused until the date changes.");

    return 0;
}
