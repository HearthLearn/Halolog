add_executable(halolog_multi_logger main.cpp)
target_link_libraries(halolog_multi_logger PRIVATE halolog::halolog)
set_target_properties(
    halolog_multi_logger
    PROPERTIES FOLDER "Examples/Usage"
)
