//
// Created by new on 2026/9/21.
//

#ifndef HALOLOG_UTILS_H
#define HALOLOG_UTILS_H

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace halolog
{
    inline bool
    ttyColorSupport(std::ostream &stream)
    {
#ifdef _WIN32
        DWORD standard_handle = 0;

        if (&stream == &std::cout)
        {
            standard_handle = STD_OUTPUT_HANDLE;
        }
        else if (&stream == &std::cerr || &stream == &std::clog)
        {
            standard_handle = STD_ERROR_HANDLE;
        }
        else
        {
            return false;
        }

        HANDLE handle = GetStdHandle(standard_handle);
        if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        DWORD mode = 0;
        if (!GetConsoleMode(handle, &mode))
        {
            return false;
        }

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        return SetConsoleMode(handle, mode) != 0;
#else
        int file_descriptor = -1;

        if (&stream == &std::cout)
        {
            file_descriptor = STDOUT_FILENO;
        }
        else if (&stream == &std::cerr || &stream == &std::clog)
        {
            file_descriptor = STDERR_FILENO;
        }
        else
        {
            return false;
        }

        return ::isatty(file_descriptor) != 0;
#endif
    }

}

#endif // HALOLOG_UTILS_H
