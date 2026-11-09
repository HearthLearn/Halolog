#include "includes.h"

TEST_CASE("flush manual: no auto flush", "[flush]")
{
    auto name = uniqueName("fl_man");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);

    for (int i = 0; i < 100; ++i)
        HLOG(logger, Info, "msg {}", i);

    REQUIRE(appender->msgCount() == 100);
    REQUIRE(appender->flushCount() == 0);

    logger.flush();
    REQUIRE(appender->flushCount() == 1);
}

TEST_CASE("flush every: flush after each message", "[flush]")
{
    auto name = uniqueName("fl_every");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>(halolog::FlushConfig::every());
    logger.addAppender(appender);

    for (int i = 0; i < 50; ++i)
        HLOG(logger, Info, "msg {}", i);

    REQUIRE(appender->msgCount() == 50);
    REQUIRE(appender->flushCount() == 50);
}

TEST_CASE("flush on level: only triggers at or above threshold", "[flush]")
{
    auto name = uniqueName("fl_lvl");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>(
        halolog::FlushConfig::onLevel(Warning));
    logger.addAppender(appender);

    HLOG(logger, Debug, "no flush");
    HLOG(logger, Info, "no flush");
    HLOG(logger, Warning, "flush");
    HLOG(logger, Error, "flush");
    HLOG(logger, Info, "no flush");
    HLOG(logger, Fatal, "flush");

    REQUIRE(appender->msgCount() == 6);
    REQUIRE(appender->flushCount() == 3);
}

TEST_CASE("flush on count: triggers every N messages", "[flush]")
{
    auto name = uniqueName("fl_cnt");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>(halolog::FlushConfig::onCount(25));
    logger.addAppender(appender);

    for (int i = 0; i < 100; ++i)
        HLOG(logger, Info, "msg {}", i);

    REQUIRE(appender->msgCount() == 100);
    REQUIRE(appender->flushCount() == 4);

    SECTION("extra messages below threshold don't trigger")
    {
        for (int i = 0; i < 10; ++i)
            HLOG(logger, Info, "extra {}", i);
        REQUIRE(appender->flushCount() == 4);
    }

    SECTION("manual flush resets counter")
    {
        logger.flush();
        REQUIRE(appender->flushCount() == 5);
    }
}

TEST_CASE("flush propagates to all appenders", "[flush]")
{
    auto name = uniqueName("fl_multi");
    auto &logger = halolog::Logger::registry(name);
    auto a1 = std::make_shared<TestAppender>();
    auto a2 = std::make_shared<TestAppender>();
    logger.addAppender(a1);
    logger.addAppender(a2);

    logger.flush();

    REQUIRE(a1->flushCount() == 1);
    REQUIRE(a2->flushCount() == 1);
}

TEST_CASE("flush on count: counter resets after manual flush", "[flush]")
{
    auto name = uniqueName("fl_cnt_rst");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>(halolog::FlushConfig::onCount(10));
    logger.addAppender(appender);

    for (int i = 0; i < 5; ++i)
        HLOG(logger, Info, "batch1 {}", i);
    REQUIRE(appender->flushCount() == 0);

    logger.flush();
    REQUIRE(appender->flushCount() == 1);

    for (int i = 0; i < 10; ++i)
        HLOG(logger, Info, "batch2 {}", i);
    REQUIRE(appender->flushCount() == 2);

    for (int i = 0; i < 9; ++i)
        HLOG(logger, Info, "batch3 {}", i);
    REQUIRE(appender->flushCount() == 2);

    HLOG(logger, Info, "trigger");
    REQUIRE(appender->flushCount() == 3);
}
