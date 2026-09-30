add_executable(halolog_console main.cpp)
target_link_libraries(halolog_console PRIVATE halolog::halolog)
set_target_properties(
    halolog_console
    PROPERTIES FOLDER "Examples/Usage"
)