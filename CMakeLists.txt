cmake_minimum_required(VERSION 3.2)

project(halolog VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(halolog INTERFACE
        src/include/logger.h
        src/include/record.h
        src/include/appender/appender.h
        src/include/appender/console_appender.h
        src/include/utils.h
        src/include/appender/file_appender.h
        src/include/appender/dailyfile_appender.h)

target_include_directories(halolog
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
)

# MSVC: conformant preprocessor (required for __VA_OPT__) + UTF-8 source
target_compile_options(halolog INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /Zc:preprocessor>
)

option(BUILD_EXAMPLE "Build Example" ON)
if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif ()
