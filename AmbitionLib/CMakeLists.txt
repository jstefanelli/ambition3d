cmake_minimum_required (VERSION 3.21)

add_subdirectory(deps/glm)
find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_library (AmbitionLib SHARED "library_main.cpp"  "include/ambition/renderTask.h" "include/ambition/window.h" "include/ambition/gl/renderer.h" "include/ambition/resources/resources.h" "include/ambition/logic/element.h" "include/ambition/logic/scene.h"  "include/ambition/transform.h" "src/transform.cpp" "src/logic/element.cpp" "include/ambition/logic/camera.h" "src/logic/camera.cpp" "src/logic/component.cpp" include/ambition/gl/texture.h src/gl/texture.cpp include/ambition/gl/utils.h include/ambition/logic/components/mesh_renderer.h include/ambition/gl/mesh.h include/ambition/gl/buffer.h src/gl/buffer.cpp)
target_compile_definitions (AmbitionLib PRIVATE AMBITION_EXPORTS)
target_include_directories (AmbitionLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries (AmbitionLib PUBLIC glm::glm "CoroutineLib")
target_link_libraries (AmbitionLib PRIVATE SDL2::SDL2 GLEW::GLEW)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(AmbitionLib PUBLIC AMBITIONLIB_GL_STRICT)
endif()

if(MSVC)
    target_compile_options(AmbitionLib PRIVATE /WX /W3)
else()
    target_compile_options(AmbitionLib PRIVATE -Wall -Wextra -Werror -pedantic-errors)
endif()

set_property (TARGET AmbitionLib PROPERTY CXX_STANDARD 20)
set_property (TARGET AmbitionLib PROPERTY CXX_STANDARD_REQUIRED TRUE)