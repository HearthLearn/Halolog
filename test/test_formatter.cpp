#include "includes.h"

std::string formatWith(const std::string &pattern, const halolog::Record &record)
{
    halolog::Formatter fmt(pattern);
    return fmt.format(record);
}

halolog::Record makeRecord(
    halolog::LogLevel level = Info,
    const std::string &loggerName = "test",
    const std::string &message = "hello")
{
    return halolog::Record(level, loggerName, message);
}

TEST_CASE("message field", "[formatter]")
{
    auto r = makeRecord(Info, "test", "hello world");
    REQUIRE(formatWith("{m}", r) == "hello world");
}

TEST_CASE("level field", "[formatter]")
{
    auto r = makeRecord(Warning);
    REQUIRE(formatWith("{p}", r) == "WARNING");
}

TEST_CASE("short level field", "[formatter]")
{
    auto r = makeRecord(Info);
    REQUIRE(formatWith("{P}", r) == "I");

    auto rw = makeRecord(Warning);
    REQUIRE(formatWith("{P}", rw) == "W");
}

TEST_CASE("logger name field", "[formatter]")
{
    auto r = makeRecord(Info, "mylogger", "msg");
    REQUIRE(formatWith("{N}", r) == "mylogger");
}

TEST_CASE("newline field", "[formatter]")
{
    auto r = makeRecord();
    REQUIRE(formatWith("{m}{n}", r) == "hello\n");
}

TEST_CASE("literal text preserved", "[formatter]")
{
    auto r = makeRecord();
    REQUIRE(formatWith("[prefix] {m} [suffix]", r) == "[prefix] hello [suffix]");
}

TEST_CASE("escaped braces", "[formatter]")
{
    auto r = makeRecord();
    REQUIRE(formatWith("{{literal}}", r) == "{literal}");
}

TEST_CASE("datetime field produces output", "[formatter]")
{
    auto r = halolog::Record(
            Info,
            "test",
            "Hello",
            std::chrono::current_zone()->to_sys(
                std::chrono::local_days{std::chrono::year{2026} /
                    std::chrono::month{10} /std::chrono::day{24}} +
                std::chrono::hours{12}),
            std::this_thread::get_id(),
            "","","",0);
    auto result = formatWith("{d:%Y-%m-%d}", r);
    REQUIRE(result.size() == 10);
    REQUIRE(result == "2026-10-24");
}

TEST_CASE("format spec applied to level", "[formatter]")
{
    auto r = makeRecord(Info);
    auto result = formatWith("{p:<10}", r);
    REQUIRE(result == "INFO      ");
    REQUIRE(result.size() == 10);
}

TEST_CASE("source location fields", "[formatter]")
{
    auto r = makeRecord();
    auto fileName = formatWith("{F}", r);
    auto line = formatWith("{L}", r);
    auto func = formatWith("{M}", r);

    REQUIRE_FALSE(fileName.empty());
    REQUIRE_FALSE(line.empty());
    REQUIRE_FALSE(func.empty());
}

TEST_CASE("unknown field throws", "[formatter]")
{
    REQUIRE_THROWS_AS(halolog::Formatter("{X}"), std::invalid_argument);
}

TEST_CASE("unclosed brace throws", "[formatter]")
{
    REQUIRE_THROWS_AS(halolog::Formatter("{m"), std::invalid_argument);
}

TEST_CASE("invalid format spec throws", "[formatter]")
{
    REQUIRE_THROWS_AS(halolog::Formatter("{m:invalid}"), std::invalid_argument);
}

TEST_CASE("line break rejects format spec", "[formatter]")
{
    REQUIRE_THROWS_AS(halolog::Formatter("{n:>1}"), std::invalid_argument);
}
TEST_CASE("thread id field", "[formatter]")
{
    auto r = makeRecord();
    auto result = formatWith("{t}", r);
    REQUIRE_FALSE(result.empty());
}

TEST_CASE("full file path field", "[formatter]")
{
    auto r = makeRecord();
    auto result = formatWith("{fp}", r);
    REQUIRE_FALSE(result.empty());
}

TEST_CASE("empty message", "[formatter]")
{
    auto r = makeRecord(Info, "test", "");
    REQUIRE(formatWith("{m}", r).empty());
}

TEST_CASE("format spec applied to message", "[formatter]")
{
    auto r = makeRecord(Info, "test", "hi");
    auto result = formatWith("{m:>10}", r);
    REQUIRE(result == "        hi");
    REQUIRE(result.size() == 10);
}

TEST_CASE("combined pattern", "[formatter]")
{
    auto r = makeRecord(Error, "myapp", "boom");
    auto result = formatWith("[{p}] {N}: {m}", r);
    REQUIRE(result == "[ERROR] myapp: boom");
}
