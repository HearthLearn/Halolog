add_executable(halolog_rolling_file main.cpp)
target_link_libraries(halolog_rolling_file PRIVATE halolog::halolog)
set_target_properties(
    halolog_rolling_file
    PROPERTIES FOLDER "Examples/Usage"
)
