add_subdirectory(console)
add_subdirectory(rolling_file)

if(HALOLOG_BUILD_QT_EXAMPLE)
    add_subdirectory(qt)
endif()
