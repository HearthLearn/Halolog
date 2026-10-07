#include <halolog/appender/console_appender.h>
#include <halolog/halolog.h>

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

    HLOG_T("plain text");
    HLOG_D("types: string={}, char={}, bool={}, signed={}, unsigned={}, float={}, pointer={}",
           std::string("Halo"), 'A', true, -42, 42U, 3.1415926, pointer);
    HLOG_I("format: hex={:#X}, padded={:06}, precision={:.2f}",
           255, 42, 3.1415926);

    HLOG_W() << "stream: value=" << value;
    HLOG_E("mixed: id={}", 1001)
        << ", hex=" << std::showbase << std::uppercase << std::hex << 255
        << ", fixed=" << std::fixed << std::setprecision(2) << 3.1415926;

    const User user{1001, "Halo", true};
    HLOG_D("enum=") << MissionState::Completed;
    HLOG_I("custom type=") << user;

    return 0;
}
