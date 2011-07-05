cmake_minimum_required(VERSION 2.8)
# Set policy: link_directories is relative to source dir
cmake_policy(SET CMP0015 NEW)

set(CMAKE_C_FLAGS "-Wall -g")

include_directories("include")
include_directories("include/gari")
include_directories("include/si")
include_directories("include/lua")
include_directories("include/raku")
include_directories("include/eruta")

link_directories("src/gari")
link_directories("src/si")
link_directories("src/lua")
link_directories("src/raku")

add_subdirectory("src/gari")
add_subdirectory("src/si")
add_subdirectory("src/lua")
add_subdirectory("src/raku")
add_subdirectory("src/eruta")


enable_testing()
# Let ctest run valgrind
set(CMAKE_CTEST_OPTIONS --output-on-failure)
set(CTEST_OUTPUT_ON_FAILURE TRUE)
set(CTEST_MEMORYCHECK_COMMAND:FILEPATH "/usr/bin/valgrind")
add_subdirectory("test/si")
add_subdirectory("test/gari")
add_subdirectory("test/raku")









