#include "includes.h"

TEST_CASE("appender level filtering", "[appender]")
{
    auto name = uniqueName("app_lvl");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    appender->setMinLevel(Warning);
    logger.addAppender(appender);

    HLOG(logger, Trace, "skip");
    HLOG(logger, Debug, "skip");
    HLOG(logger, Info, "skip");
    HLOG(logger, Warning, "pass");
    HLOG(logger, Error, "pass");
    HLOG(logger, Fatal, "pass");

    REQUIRE(appender->msgCount() == 3);
}

TEST_CASE("filtered records do not trigger automatic flush", "[appender]")
{
    auto appender = std::make_shared<TestAppender>(halolog::FlushConfig::every());
    appender->setMinLevel(Warning);

    REQUIRE(appender->append(makeTestRecord(Info)));
    REQUIRE(appender->msgCount() == 0);
    REQUIRE(appender->flushCount() == 0);

    REQUIRE(appender->append(makeTestRecord(Warning)));
    REQUIRE(appender->msgCount() == 1);
    REQUIRE(appender->flushCount() == 1);
}
TEST_CASE("appender custom formatter", "[appender]")
{
    auto name = uniqueName("app_fmt");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    appender->setFormatter(std::make_unique<halolog::Formatter>("[{p}] {m}"));
    logger.addAppender(appender);

    HLOG(logger, Info, "test message");

    auto msgs = appender->messages();
    REQUIRE(msgs.size() == 1);
    REQUIRE(msgs[0] == "[INFO] test message");
}

TEST_CASE("multi appender dispatch", "[appender]")
{
    auto name = uniqueName("multi");
    auto &logger = halolog::Logger::registry(name);
    auto a1 = std::make_shared<TestAppender>();
    auto a2 = std::make_shared<TestAppender>();
    logger.addAppender(a1);
    logger.addAppender(a2);
    HLOG(logger, Info, "msg");
    REQUIRE(a1->msgCount() == 1);
    REQUIRE(a2->msgCount() == 1);
}

TEST_CASE("console appender writes to stream", "[appender]")
{
    auto name = uniqueName("console_w");
    auto &logger = halolog::Logger::registry(name);
    std::ostringstream oss;
    auto appender = std::make_shared<halolog::ConsoleAppender>(oss, false);
    appender->setFormatter(std::make_unique<halolog::Formatter>("{m}"));
    logger.addAppender(appender);

    HLOG(logger, Info, "console test");

    REQUIRE(oss.str() == "console test");
}

TEST_CASE("console appender no color on non-terminal", "[appender]")
{
    auto name = uniqueName("console_c");
    auto &logger = halolog::Logger::registry(name);
    std::ostringstream oss;
    auto appender = std::make_shared<halolog::ConsoleAppender>(oss, true);
    appender->setFormatter(std::make_unique<halolog::Formatter>("{m}"));
    logger.addAppender(appender);

    HLOG(logger, Info, "color test");

    auto output = oss.str();
    REQUIRE(output.find("\033[") == std::string::npos);
    REQUIRE(output == "color test");
}
