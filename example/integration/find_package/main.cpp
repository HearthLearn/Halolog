#include <halolog/appender/console_appender.h>
#include <halolog/logger.h>

int main()
{
    auto &logger = halolog::Logger::instance();
    logger.addAppender(std::make_shared<halolog::ConsoleAppender>());

    HLOG_I("Halolog was located with find_package.");
    return 0;
}
