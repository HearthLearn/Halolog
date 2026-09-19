//
// Created by new on 2026/9/18.
//

#include "logger.h"
#include "appender/console_appender.h"

#include <cstdint>
#include <iomanip>
#include <string>

enum LegacyStatus
{
    LegacyReady = 7
};

struct User
{
    int id;
    std::string name;
};

std::ostream &operator<<(std::ostream &stream, const User &user)
{
    return stream << "User{id=" << user.id << ", name=" << user.name << '}';
}

int main()
{
    auto &logger = halolog::Logger::instance();
    auto console = std::make_shared<halolog::ConsoleAppender>();
    logger.addAppender(console);

    HLOG_D("sample test");

    const char *cString = "C string";
    int pointedValue = 42;

    HLOG_D("strings: ")
        << "string literal"
        << ", std::string=" << std::string("standard string")
        << ", const char*=" << cString
        << ", char=" << 'C';

    HLOG_D("integers: ")
        << "short=" << static_cast<short>(-12)
        << ", unsigned short=" << static_cast<unsigned short>(12)
        << ", int=" << -123
        << ", unsigned int=" << 123U
        << ", long=" << 123L
        << ", unsigned long=" << 123UL
        << ", int64=" << std::int64_t{-123456789}
        << ", uint64=" << std::uint64_t{123456789};

    HLOG_D("floating point: ")
        << "float=" << 1.25F
        << ", double=" << 3.1415926
        << ", long double=" << 2.5L;

    HLOG_D("boolean: ")
        << "default=" << true
        << ", boolalpha=" << std::boolalpha << true
        << ", reset=" << std::noboolalpha << false;

    HLOG_D("pointers: ")
        << "object=" << &pointedValue
        << ", void*=" << static_cast<const void *>(&pointedValue)
        << ", nullptr=" << nullptr;

    HLOG_D("enum and custom type: ")
        << "legacy enum=" << LegacyReady
        << ", custom=" << User{1, "Alice"};

    HLOG_D("format manipulators: ")
        << "hex=" << std::hex << 255
        << ", decimal=" << std::dec << 255
        << ", fixed=" << std::fixed << std::setprecision(2) << 3.1415926
        << ", scientific=" << std::scientific << 3.1415926
        << ", padded=" << std::setfill('0') << std::setw(5) << 42;

    HLOG_D("ostream manipulators:")
        << std::endl
        << "second line"
        << std::flush;

    return 0;
}
