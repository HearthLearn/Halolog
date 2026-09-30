cmake_minimum_required(VERSION 3.23)

project(halolog_qt LANGUAGES CXX)

set(HALOLOG_QT_SDK
    "D:/Qt/6.6.3/msvc2019_64"
    CACHE PATH
    "Path to the Qt SDK"
)
list(PREPEND CMAKE_PREFIX_PATH "${HALOLOG_QT_SDK}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(halolog_qt main.cpp)

target_compile_definitions(
    halolog_qt
    PRIVATE
    QT_MESSAGELOGCONTEXT
)

target_link_libraries(
    halolog_qt
    PRIVATE
    halolog::halolog
    Qt${QT_VERSION_MAJOR}::Core
)
set_target_properties(
    halolog_qt
    PROPERTIES FOLDER "Examples/Usage"
)
