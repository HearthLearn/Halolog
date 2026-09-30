cmake_minimum_required(VERSION 3.23)

project(halolog_find_package LANGUAGES CXX)

find_package(halolog CONFIG REQUIRED)

add_executable(halolog_find_package main.cpp)
target_link_libraries(
    halolog_find_package
    PRIVATE halolog::halolog
)
