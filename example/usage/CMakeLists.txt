add_subdirectory(console)
add_subdirectory(rolling_file)
add_subdirectory(multi_logger)

if(HALOLOG_BUILD_QT_EXAMPLE)
    add_subdirectory(qt)
endif()
