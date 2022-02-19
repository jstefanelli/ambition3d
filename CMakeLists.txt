# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("CoroutineLib")

option(CRLIB_USE_BOOST_QUEUE "Link CoroutineLib to boost::lockfree for a stable lock-free queue implementation" ON)
if (CRLIB_USE_BOOST_QUEUE)
set_directory_properties(PROPERTIES "CRLIB_USE_BOOST_QUEUE" 1)
else()
set_directory_properties(PROPERTIES "CRLIB_USE_BOOST_QUEUE" 0)
endif()

# Include sub-projects.
add_subdirectory ("CoroutineLib")
add_subdirectory ("CoroutineTest")
add_subdirectory ("AmbitionLib")
add_subdirectory ("AmbitionTest")
