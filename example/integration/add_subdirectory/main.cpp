#include <halolog/appender/console_appender.h>
#include <halolog/halolog.h>

int main()
{
    auto &logger = halolog::Logger::instance();
    logger.addAppender(std::make_shared<halolog::ConsoleAppender>());

    HLOG_I("Halolog was integrated with add_subdirectory.");
    return 0;
}
