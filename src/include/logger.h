//
// Created by new on 2026/9/18.
//

#ifndef LOGGER_H
#define LOGGER_H
#include <memory>
#include "appender/appender.h"
#include <vector>
#include "record.h"

namespace halolog
{
    class Logger
    {
    public:
        static Logger &instance()
        {
            static Logger logger;
            return logger;
        }

        void addAppender(std::shared_ptr<Appender> appender)
        {
            mAppenders.push_back(std::move(appender));
        }

        void setMinLevel(LogLevel level)
        {
            mMinLevel = level;
        }

        LogLevel minLevel() const
        {
            return mMinLevel;
        }

        void operator+=(const Record &record)
        {
            for (const auto &appender : mAppenders)
            {
                if (appender)
                {
                    appender->append(record);
                }
            }
        }

    private:
        Logger() {};
        LogLevel mMinLevel{LogLevel::Trace};
        std::vector<std::shared_ptr<Appender>> mAppenders;
    };

};

// TODO: 改成 HLOG（logger* level message）
// 支持 HLOG() << "xxx"的一个思路是
// 一个方案是record持有logger  HLOG() 展开返回结果为 record ，record析构时写入
// 一个方案是plog的方式，利用运算符优先级 展开为 logger += record << "xxx"; 等效于 logger += （record << "xxx"） 然后重写record的<<运算符和logger的+=运算符

// 提前判断等级过滤，减少Record构造开销
#define HLOG(logger, level, message) \
    if (level < logger.minLevel())   \
    {                                \
        ;                            \
    }                                \
    else                             \
        logger += halolog::Record(level, message)

#define DEF_HLOG(level, message) HLOG(halolog::Logger::instance(), level, message)

#define HLOG_T(message) DEF_HLOG(halolog::LogLevel::Trace, message)
#define HLOG_D(message) DEF_HLOG(halolog::LogLevel::Debug, message)
#define HLOG_I(message) DEF_HLOG(halolog::LogLevel::Info, message)
#define HLOG_W(message) DEF_HLOG(halolog::LogLevel::Warning, message)
#define HLOG_E(message) DEF_HLOG(halolog::LogLevel::Error, message)
#define HLOG_F(message) DEF_HLOG(halolog::LogLevel::Fatal, message)

#endif // LOGGER_H
