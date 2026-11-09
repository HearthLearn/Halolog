#include "includes.h"

constexpr int kThreads = 8;
constexpr int kMsgsPerThread = 500;

TEST_CASE("concurrent message count", "[thread]")
{
    auto name = uniqueName("mt_count");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);

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

    REQUIRE(appender->msgCount() == kThreads * kMsgsPerThread);
}

TEST_CASE("concurrent message integrity", "[thread]")
{
    auto name = uniqueName("mt_integ");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);

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

    auto msgs = appender->messages();
    REQUIRE(static_cast<int>(msgs.size()) == kThreads * kMsgsPerThread);

    int garbled = 0;
    for (const auto &m : msgs)
    {
        if (m.size() != 84 || m[0] != 'T' || m[3] != '|')
        {
            ++garbled;
            continue;
        }
        const char tens = m[1];
        const char ones = m[2];
        const int tid = (tens - '0') * 10 + (ones - '0');
        if (tid < 0 || tid >= kThreads)
        {
            ++garbled;
            continue;
        }

        const char expectedFill = static_cast<char>('A' + tid);
        for (size_t j = 4; j < m.size(); ++j)
        {
            if (m[j] != expectedFill)
            {
                ++garbled;
                break;
            }
        }
    }
    REQUIRE(garbled == 0);
}

TEST_CASE("concurrent no interleaving", "[thread]")
{
    auto name = uniqueName("mt_interl");
    auto &logger = halolog::Logger::registry(name);
    auto appender = std::make_shared<TestAppender>();
    logger.addAppender(appender);

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

    auto msgs = appender->messages();
    std::vector<int> perThread(kThreads, 0);
    std::vector<std::vector<int>> sequenceCounts(
        kThreads, std::vector<int>(kMsgsPerThread, 0));
    int bad = 0;
    for (const auto &m : msgs)
    {
        if (m.size() != 8 || m[0] != 'T' || m[3] != '-')
        {
            ++bad;
            continue;
        }

        const int tid = (m[1] - '0') * 10 + (m[2] - '0');
        const int sequence = (m[4] - '0') * 1000 +
                             (m[5] - '0') * 100 +
                             (m[6] - '0') * 10 +
                             (m[7] - '0');
        if (tid < 0 || tid >= kThreads ||
            sequence < 0 || sequence >= kMsgsPerThread)
        {
            ++bad;
            continue;
        }

        ++perThread[tid];
        ++sequenceCounts[tid][sequence];
    }
    REQUIRE(bad == 0);
    for (int t = 0; t < kThreads; ++t)
    {
        REQUIRE(perThread[t] == kMsgsPerThread);
        for (int sequence = 0; sequence < kMsgsPerThread; ++sequence)
            REQUIRE(sequenceCounts[t][sequence] == 1);
    }
}