# Ambition3D
My latest collection of experiments in the realm of 3D rendering

# Structure

The project is managed with CMake.

For the exact sturcture (and all the build options which are not described here) check out the various *CMakeLists.txt* files.

## CoroutineLib

This is the latest iteration of my C++ Task library. It offers basic functionality inspired by C#'s Task system. 
It's based on C++ 20 coroutines, which forces the use of rather modern GCC or MSVC.

## AmbitrionLib

This is the main engine module. The goal for this is to be a basic 3D rendering engine, built on top of SDL2 and OpenGL.

## AmbitionTest

This is the test application (as in, a tiny executable used for manual testing)

# Dependencies

Required dependencies:

 - A cpp-20 compatible compiler
 - SDL2
 - GLEW

Optional dependencies:

 - Boost::Lockfree (enabled by default, used in the 'CoroutineLib' module. Check its *CMakeLists.txt* file for more info)

The project has been compiled succesfully on Windows with *MSVC 2019, 2022* and on macOS with *GCC*.

# License

Do whatever you want! I'd appreciate if my name (or a link to this repository) is somewhere in your project, but that's not required.
