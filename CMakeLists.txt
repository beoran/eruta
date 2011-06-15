cmake_minimum_required(VERSION 2.8)
# Set policy: link_directories is relative to source dir
cmake_policy(SET CMP0015 NEW)

set(CMAKE_C_FLAGS "-Wall -g")

include_directories("include")
link_directories("src/re")

add_subdirectory("src/re")

enable_testing()
# Let ctest run valgrind
set(CTEST_OUTPUT_ON_FAILURE TRUE)
set(CTEST_MEMORYCHECK_COMMAND:FILEPATH "/usr/bin/valgrind")
add_subdirectory("test/re")







