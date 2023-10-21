cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
project (IceNfsd CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_CXX_FLAGS "/MP /WX /W4 /EHsc /sdl /guard:cf /GS")
set (CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
set (CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /D NDEBUG")

set (Boost_USE_STATIC_LIBS on)
set (Boost_USE_MULTITHREADED on)
set (Boost_USE_STATIC_RUNTIME on)
find_package (Boost 1.72.0 REQUIRED COMPONENTS log program_options unit_test_framework)
add_subdirectory (src)
add_subdirectory (tests)