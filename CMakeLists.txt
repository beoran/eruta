cmake_minimum_required(VERSION 2.6)

set(CMAKE_C_FLAGS "-Wall -g")

include_directories("include")
link_directories("src/re")

add_subdirectory("src/re")

enable_testing()
add_subdirectory("test/re")







