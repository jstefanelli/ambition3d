cmake_minimum_required (VERSION 3.20)


add_subdirectory(deps/glm)
find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_library (AmbitionLib SHARED "library_main.cpp")
target_include_directories (AmbitionLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions (AmbitionLib PRIVATE AMBITION_EXPORTS)
target_link_libraries (AmbitionLib PUBLIC glm::glm "CoroutineLib")
target_link_libraries (AmbitionLib PRIVATE SDL2::SDL2 GLEW::GLEW)