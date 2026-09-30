cmake_minimum_required(VERSION 3.23)

project(halolog_add_subdirectory LANGUAGES CXX)

set(HALOLOG_SOURCE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../../.."
)

add_subdirectory(
    "${HALOLOG_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/halolog"
    EXCLUDE_FROM_ALL
)

add_executable(halolog_add_subdirectory main.cpp)
target_link_libraries(
    halolog_add_subdirectory
    PRIVATE halolog::halolog
)
