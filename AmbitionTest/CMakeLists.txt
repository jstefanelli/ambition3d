cmake_minimum_required(VERSION 3.21)

add_executable(AmbitionTest "src/main.cpp")
target_link_libraries(AmbitionTest PRIVATE "AmbitionLib")

set_property (TARGET AmbitionTest PROPERTY CXX_STANDARD 20)
set_property (TARGET AmbitionTest PROPERTY CXX_STANDARD_REQUIRED TRUE)

add_custom_command(TARGET AmbitionTest POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:AmbitionTest> $<TARGET_FILE_DIR:AmbitionTest>
  COMMAND_EXPAND_LISTS
)