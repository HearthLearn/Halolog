#include "includes.h"

TEST_CASE("HLOG_* convenience macros use default logger", "[macros]")
{
    auto &logger = halolog::Logger::get();
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);

    HLOG_T("trace {}", 1);
    HLOG_D("debug {}", 2);
    HLOG_I("info {}", 3);
    HLOG_W("warn {}", 4);
    HLOG_E("error {}", 5);
    HLOG_F("fatal {}", 6);

    REQUIRE(appender->msgCount() == 6);

    auto msgs = appender->messages();
    REQUIRE(msgs[0] == "trace 1");
    REQUIRE(msgs[1] == "debug 2");
    REQUIRE(msgs[2] == "info 3");
    REQUIRE(msgs[3] == "warn 4");
    REQUIRE(msgs[4] == "error 5");
    REQUIRE(msgs[5] == "fatal 6");
}

TEST_CASE("HLOGN_* macros use named logger", "[macros]")
{
    auto name = uniqueName("macro_n");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);

    HLOGN_I(name, "hello {}", 42);

    REQUIRE(appender->msgCount() == 1);
    REQUIRE(appender->messages()[0] == "hello 42");
}

TEST_CASE("stream-only style", "[macros]")
{
    auto name = uniqueName("macro_stream");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);

    HLOG(logger, Info) << "stream " << 123;

    REQUIRE(appender->msgCount() == 1);
    REQUIRE(appender->messages()[0] == "stream 123");
}

TEST_CASE("mixed style: format + stream", "[macros]")
{
    auto name = uniqueName("macro_mix");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);

    HLOG(logger, Info, "prefix {}", 1) << " suffix";

    REQUIRE(appender->msgCount() == 1);
    REQUIRE(appender->messages()[0] == "prefix 1 suffix");
}
TEST_CASE("filtered macro does not evaluate format arguments", "[macros]")
{
    auto &logger = halolog::Logger::registry(uniqueName("macro_filter"));
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);
    logger.setMinLevel(Warning);

    int evaluations = 0;
    HLOG(logger, Info, "value {}", ++evaluations);

    REQUIRE(evaluations == 0);
    REQUIRE(appender->msgCount() == 0);
}
