#include "includes.h"

TEST_CASE("registry creates new logger", "[logger]")
{
    auto name = uniqueName("reg");
    auto &logger = halolog::Logger::registry(name);
    REQUIRE(logger.name() == name);
}

TEST_CASE("duplicate registry throws", "[logger]")
{
    auto name = uniqueName("dup");
    halolog::Logger::registry(name);
    REQUIRE_THROWS_AS(halolog::Logger::registry(name), std::invalid_argument);
}

TEST_CASE("get retrieves registered logger", "[logger]")
{
    auto name = uniqueName("get");
    auto &created = halolog::Logger::registry(name);
    auto &retrieved = halolog::Logger::get(name);
    REQUIRE(&created == &retrieved);
}

TEST_CASE("get throws for unknown name", "[logger]")
{
    REQUIRE_THROWS_AS(halolog::Logger::get("nonexistent_logger"), std::invalid_argument);
}

TEST_CASE("default logger is named def", "[logger]")
{
    auto &d = halolog::Logger::get();
    REQUIRE(d.name() == "def");
}

TEST_CASE("logger min level is configurable", "[logger]")
{
    auto &logger = halolog::Logger::registry(uniqueName("lvl_get"));
    REQUIRE(logger.minLevel() == Trace);

    logger.setMinLevel(Error);
    REQUIRE(logger.minLevel() == Error);
}
TEST_CASE("min level filters records", "[logger]")
{
    auto name = uniqueName("lvl");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);
    logger.setMinLevel(Warning);

    HLOG(logger, Info, "should be skipped");
    HLOG(logger, Warning, "should pass");
    HLOG(logger, Error, "should pass too");

    REQUIRE(appender->msgCount() == 2);
}
