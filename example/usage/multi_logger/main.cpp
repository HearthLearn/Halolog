#include <halolog/appender/console_appender.h>
#include <halolog/appender/rollingfile_appender.h>
#include <halolog/halolog.h>

int main()
{
    auto console = std::make_shared<halolog::ConsoleAppender>();

    // default logger: console, Trace and above
    auto &def = halolog::Logger::get();
    def.addAppender(console);

    // "net" logger: console only, Debug and above
    auto &net = halolog::Logger::registry("net");
    net.setMinLevel(halolog::LogLevel::Debug);
    net.addAppender(console);

    // "db" logger: console + file, Info and above
    auto &db = halolog::Logger::registry("db");
    db.setMinLevel(halolog::LogLevel::Info);
    db.addAppender(console);
    db.addAppender(std::make_shared<halolog::RollingFileAppender>(
        "logs/db_{:%Y-%m-%d}.log"));

    HLOG_I("app started");

    HLOGN_D("net", "connecting to {}:{}", "127.0.0.1", 8080);
    HLOGN_I("net", "connection established");

    HLOGN_I("db", "opening database");
    HLOGN_W("db", "slow query: {} ms", 523);
    HLOGN_D("db", "this debug line is filtered out by db's min level");

    HLOG_I("shutting down");

    return 0;
}
