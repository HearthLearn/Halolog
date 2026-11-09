include(FetchContent)
FetchContent_Declare(
    Catch2
    URL      https://github.com/catchorg/Catch2/archive/refs/tags/v3.7.1.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(halolog_tests
    test_logger.cpp
    test_formatter.cpp
    test_appender.cpp
    test_flush.cpp
    test_thread_safety.cpp
    test_file.cpp
    test_macros.cpp
)

target_link_libraries(halolog_tests PRIVATE halolog::halolog Catch2::Catch2WithMain)

catch_discover_tests(halolog_tests)
