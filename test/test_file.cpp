#include "includes.h"

static fs::path testDir()
{
    return fs::temp_directory_path() / ("halolog_catch2_test_" + uniqueName("run"));
}

TEST_CASE("file appender reports open failure", "[file]")
{
    auto dir = testDir() / "open_failure";
    fs::create_directories(dir);
    auto blocker = dir / "not_a_directory";
    std::ofstream(blocker).put('x');

    halolog::RollingFileAppender appender((blocker / "test.log").string());
    REQUIRE_FALSE(appender.append(makeTestRecord()));
}

TEST_CASE("readLines reports missing files", "[file]")
{
    REQUIRE_THROWS_AS(readLines(testDir() / "missing.log"), std::runtime_error);
}
TEST_CASE("file completeness: all messages written", "[file]")
{
    auto dir = testDir() / "completeness";
    fs::remove_all(dir);
    fs::create_directories(dir);
    auto logFile = dir / "test.log";

    auto name = uniqueName("f_complete");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<halolog::RollingFileAppender>(logFile.string());
    appender->setFormatter(std::make_unique<halolog::Formatter>("{m}{n}"));
    logger.addAppender(appender);

    constexpr int kThreads = 8;
    constexpr int kMsgsPerThread = 500;

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; ++t)
    {
        threads.emplace_back([&logger, t]()
                             {
            for (int i = 0; i < kMsgsPerThread; ++i)
                HLOG(logger, Info, "T{:02d}-{:04d}", t, i); });
    }
    for (auto &th : threads)
        th.join();

    appender->close();
    auto lines = readLines(logFile);
    fs::remove_all(dir);

    REQUIRE(static_cast<int>(lines.size()) == kThreads * kMsgsPerThread);
}

TEST_CASE("file integrity: no garbled lines", "[file]")
{
    auto dir = testDir() / "integrity";
    fs::remove_all(dir);
    fs::create_directories(dir);
    auto logFile = dir / "test.log";

    auto name = uniqueName("f_integ");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<halolog::RollingFileAppender>(logFile.string());
    appender->setFormatter(std::make_unique<halolog::Formatter>("{m}{n}"));
    logger.addAppender(appender);

    constexpr int kThreads = 8;
    constexpr int kMsgsPerThread = 500;

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; ++t)
    {
        threads.emplace_back([&logger, t]()
                             {
            const std::string padding(80, static_cast<char>('A' + t));
            for (int i = 0; i < kMsgsPerThread; ++i)
                HLOG(logger, Info, "T{:02d}|{}", t, padding); });
    }
    for (auto &th : threads)
        th.join();

    appender->close();
    auto lines = readLines(logFile);
    fs::remove_all(dir);

    int garbled = 0;
    for (const auto &line : lines)
    {
        if (line.size() != 84 || line[0] != 'T' || line[3] != '|')
        {
            ++garbled;
            continue;
        }
        char fill = line[4];
        for (size_t j = 5; j < line.size(); ++j)
        {
            if (line[j] != fill)
            {
                ++garbled;
                break;
            }
        }
    }
    REQUIRE(lines.size() == static_cast<size_t>(kThreads * kMsgsPerThread));
    REQUIRE(garbled == 0);
}

TEST_CASE("multi appender: memory + file", "[file]")
{
    auto dir = testDir() / "multi";
    fs::remove_all(dir);
    fs::create_directories(dir);
    auto logFile = dir / "test.log";

    auto name = uniqueName("f_multi");
    auto &logger = halolog::Logger::registry(name);
    auto mem = std::make_shared<TestAppender>();
    auto file = std::make_shared<halolog::RollingFileAppender>(logFile.string());
    file->setFormatter(std::make_unique<halolog::Formatter>("{m}{n}"));
    logger.addAppender(mem);
    logger.addAppender(file);

    constexpr int kThreads = 8;
    constexpr int kMsgsPerThread = 500;

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; ++t)
    {
        threads.emplace_back([&logger, t]()
                             {
            for (int i = 0; i < kMsgsPerThread; ++i)
                HLOG(logger, Info, "T{:02d}-{:04d}", t, i); });
    }
    for (auto &th : threads)
        th.join();

    const int expected = kThreads * kMsgsPerThread;
    REQUIRE(mem->msgCount() == expected);

    file->close();
    auto lines = readLines(logFile);
    fs::remove_all(dir);
    REQUIRE(static_cast<int>(lines.size()) == expected);
}

TEST_CASE("rolling file appender rotates on date change", "[file]")
{
    auto dir = testDir() / "rolling";
    fs::remove_all(dir);
    fs::create_directories(dir);
    auto pattern = (dir / "log_{:%Y%m%d}.txt").string();

    auto appender = std::make_shared<halolog::RollingFileAppender>(pattern);
    appender->setFormatter(std::make_unique<halolog::Formatter>("{m}{n}"));

    using namespace std::chrono;
    auto day1 = sys_days{2025y / January / 15d} + 12h;
    auto day2 = sys_days{2025y / January / 16d} + 12h;

    appender->append(makeTestRecord(Info, "roll", "day1 msg", day1));
    appender->append(makeTestRecord(Info, "roll", "day2 msg", day2));
    appender->close();

    auto zone = current_zone();
    auto local1 = zone->to_local(floor<seconds>(day1));
    auto local2 = zone->to_local(floor<seconds>(day2));
    auto fname1 = std::vformat("log_{:%Y%m%d}.txt", std::make_format_args(local1));
    auto fname2 = std::vformat("log_{:%Y%m%d}.txt", std::make_format_args(local2));

    auto file1 = dir / fname1;
    auto file2 = dir / fname2;

    REQUIRE(file1 != file2);
    REQUIRE(fs::exists(file1));
    REQUIRE(fs::exists(file2));

    auto lines1 = readLines(file1);
    auto lines2 = readLines(file2);
    REQUIRE(lines1.size() == 1);
    REQUIRE(lines2.size() == 1);
    REQUIRE(lines1[0] == "day1 msg");
    REQUIRE(lines2[0] == "day2 msg");

    fs::remove_all(dir);
}

TEST_CASE("rolling file appender appends within same period", "[file]")
{
    auto dir = testDir() / "rolling_same";
    fs::remove_all(dir);
    fs::create_directories(dir);
    auto pattern = (dir / "log_{:%Y%m%d}.txt").string();

    auto appender = std::make_shared<halolog::RollingFileAppender>(pattern);
    appender->setFormatter(std::make_unique<halolog::Formatter>("{m}{n}"));

    using namespace std::chrono;
    auto t1 = sys_days{2025y / March / 15d} + 10h;
    auto t2 = sys_days{2025y / March / 15d} + 14h;

    appender->append(makeTestRecord(Info, "roll", "morning", t1));
    appender->append(makeTestRecord(Info, "roll", "afternoon", t2));
    appender->close();

    auto zone = current_zone();
    auto local = zone->to_local(floor<seconds>(t1));
    auto fname = std::vformat("log_{:%Y%m%d}.txt", std::make_format_args(local));
    auto logFile = dir / fname;

    REQUIRE(fs::exists(logFile));
    auto lines = readLines(logFile);
    REQUIRE(lines.size() == 2);
    REQUIRE(lines[0] == "morning");
    REQUIRE(lines[1] == "afternoon");

    fs::remove_all(dir);
}

TEST_CASE("rolling file appender invalid pattern throws", "[file]")
{
    REQUIRE_THROWS_AS(
        halolog::RollingFileAppender("log_{:%Q}.txt"),
        std::invalid_argument);
}
