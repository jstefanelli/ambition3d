cmake_minimum_required (VERSION 3.21)

add_subdirectory(deps/glm)
find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_library (AmbitionLib SHARED "library_main.cpp"  "include/ambition/renderTask.h" "include/ambition/window.h" "include/ambition/gl/renderer.h" "include/ambition/resources/resources.h" "include/ambition/logic/element.h" "include/ambition/logic/scene.h" "include/ambition/logic/component.h" "include/ambition/transform.h" "src/transform.cpp" "src/logic/element.cpp" "include/ambition/logic/camera.h")
target_compile_definitions (AmbitionLib PRIVATE AMBITION_EXPORTS)
target_include_directories (AmbitionLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries (AmbitionLib PUBLIC glm::glm "CoroutineLib")
target_link_libraries (AmbitionLib PRIVATE SDL2::SDL2 GLEW::GLEW)


set_property (TARGET AmbitionLib PROPERTY CXX_STANDARD 20)
set_property (TARGET AmbitionLib PROPERTY CXX_STANDARD_REQUIRED TRUE)