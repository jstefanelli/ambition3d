cmake_minimum_required(VERSION 3.20)

add_executable(AmbitionTest "src/main.cpp")
target_link_libraries(AmbitionTest "AmbitionLib")

set_property (TARGET AmbitionTest PROPERTY CXX_STANDARD 20)
set_property (TARGET AmbitionTest PROPERTY CXX_STANDARD_REQUIRED TRUE)