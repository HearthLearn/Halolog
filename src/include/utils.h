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

inline bool ttyColorSupport(std::ostream &stream)
{
#ifdef _WIN32
    DWORD standardHandle = 0;

    if (&stream == &std::cout)
    {
        standardHandle = STD_OUTPUT_HANDLE;
    }
    else if (&stream == &std::cerr || &stream == &std::clog)
    {
        standardHandle = STD_ERROR_HANDLE;
    }
    else
    {
        return false;
    }

    HANDLE handle = GetStdHandle(standardHandle);
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
    int fileDescriptor = -1;

    if (&stream == &std::cout)
    {
        fileDescriptor = STDOUT_FILENO;
    }
    else if (&stream == &std::cerr || &stream == &std::clog)
    {
        fileDescriptor = STDERR_FILENO;
    }
    else
    {
        return false;
    }

    return ::isatty(fileDescriptor) != 0;
#endif
}

#endif // HALOLOG_UTILS_H
