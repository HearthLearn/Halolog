//
// Created by new on 2026/9/18.
//

#include "logger.h"
#include "appender/console_appender.h"

#include <iomanip>
#include <memory>
#include <string>
#include <string_view>

enum class MissionState
{
    Idle,
    Running,
    Completed
};

constexpr std::string_view toString(MissionState state)
{
    switch (state)
    {
    case MissionState::Idle:
        return "Idle";
    case MissionState::Running:
        return "Running";
    case MissionState::Completed:
        return "Completed";
    }

    return "Unknown";
}

std::ostream &operator<<(std::ostream &stream, MissionState state)
{
    return stream << toString(state);
}

struct User
{
    int id;
    std::string name;
    bool active;
};

std::ostream &operator<<(std::ostream &stream, const User &user)
{
    return stream << "User{id=" << user.id
                  << ", name=" << user.name
                  << ", active=" << std::boolalpha << user.active
                  << '}';
}

int main()
{
    auto &logger = halolog::Logger::instance();
    auto console = std::make_shared<halolog::ConsoleAppender>();
    logger.addAppender(console);

    int value = 42;
    const void *pointer = &value;

    // 1. Direct writing: plain text and std::format
    HLOG_T("plain text");
    HLOG_D("types: string={}, char={}, bool={}, signed={}, unsigned={}, float={}, pointer={}",
           std::string("Halo"), 'A', true, -42, 42U, 3.1415926, pointer);
    HLOG_I("format: hex={:#X}, binary={:#b}, padded={:06}, aligned='{:>8}', precision={:.2f}, braces={{}}",
           255, 5, 42, "right", 3.1415926);

    // 2. Streaming writing: empty message, text prefix, format prefix and flow controller.
    HLOG_W() << "empty prefix: value=" << value;
    HLOG_E("text prefix: ")
        << std::string("Halo")
        << ", char=" << 'A'
        << ", bool=" << std::boolalpha << true
        << ", signed=" << -42
        << ", unsigned=" << 42U
        << ", float=" << 3.1415926
        << ", pointer=" << pointer
        << ", null=" << nullptr;
    HLOG_F("formatted prefix: id={}", 1001)
        << ", hex=" << std::showbase << std::uppercase << std::hex << 255
        << ", fixed=" << std::fixed << std::setprecision(2) << 3.1415926
        << ", padded=" << std::noshowbase << std::dec
        << std::setfill('0') << std::setw(6) << 42
        << std::endl
        << "second line"
        << std::flush;

    // 3. Extension types: enumeration, structure
    const User user{1001, "Halo", true};
    HLOG_T("scoped enum=") << MissionState::Completed;
    HLOG_D("structure=") << user;

    return 0;
}
