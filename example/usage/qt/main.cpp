//
// Created by new on 2026/9/29.
//
#include <halolog/appender/console_appender.h>
#include <halolog/halolog.h>

#include <QCoreApplication>
#include <QDebug>
#include <QMessageLogContext>
#include <QString>

halolog::LogLevel toHalologLevel(QtMsgType type)
{
    switch (type)
    {
    case QtDebugMsg:
        return halolog::LogLevel::Debug;
    case QtInfoMsg:
        return halolog::LogLevel::Info;
    case QtWarningMsg:
        return halolog::LogLevel::Warning;
    case QtCriticalMsg:
        return halolog::LogLevel::Error;
    case QtFatalMsg:
        return halolog::LogLevel::Fatal;
    }

    return halolog::LogLevel::Info;
}

std::string toUtf8(const QString &text)
{
    const QByteArray bytes = text.toUtf8();
    return {bytes.constData(), static_cast<std::size_t>(bytes.size())};
}

void qtMessageHandler(QtMsgType type,
                      const QMessageLogContext &context,
                      const QString &message) noexcept
{
    try
    {
        const std::string fullFile = context.file != nullptr ? context.file : "";
        const std::string function = context.function != nullptr ? context.function : "";
        const std::string text = toUtf8(message);
        const auto level = toHalologLevel(type);

        halolog::log(halolog::Record(
            level,
            toUtf8(message),
            fullFile,
            std::filesystem::path(fullFile).filename().string(),
            function,
            context.line > 0
                ? static_cast<std::uint_least32_t>(context.line)
                : 0U));
    }
    catch (...)
    {
        // Qt message handlers must not let exceptions cross the callback boundary.
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    auto &logger = halolog::Logger::get();
    auto console = std::make_shared<halolog::ConsoleAppender>();
    logger.addAppender(console);

    qInstallMessageHandler(qtMessageHandler);

    qDebug() << "Qt debug message";
    qInfo() << "Qt application started:" << application.applicationFilePath();
    qWarning() << "Qt warning message";
    qCritical() << "Qt critical message";

    HLOG_I("Halolog macros and Qt logging can share the same appenders.");
    return 0;
}
