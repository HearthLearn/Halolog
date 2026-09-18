//
// Created by new on 2026/9/18.
//

#ifndef APPENDER_H
#define APPENDER_H
#include "record.h"

namespace halolog
{
    class Appender
    {
    public:
        virtual ~Appender() = default;
        virtual bool append(const Record &record) = 0;
    };

}

#endif // APPENDER_H
