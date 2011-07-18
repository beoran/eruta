cmake_minimum_required(VERSION 2.8)
# Set policy: link_directories is relative to source dir
cmake_policy(SET CMP0015 NEW)

set(CMAKE_C_FLAGS "-Wall -Wno-unused -Wno-unknown-pragmas -g -std=gnu99 -ffast-math")  
# always use gnu99, debugging, all warnings except unused and unknown pragmas.


# Find SDL
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

# Set include and lib dirs. 
include_directories(${SDL_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})

set(GY_LIBS ${LIBS} ${SDL_LIBRARY} ${SDLTTF_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${PNG_LIBRARY} ${JPEG_LIBRARY})


include_directories("include")
include_directories("include/gy")
include_directories("include/si")
include_directories("include/lua")
include_directories("include/chipmunk")
include_directories("include/raku")
include_directories("include/eruta")

link_directories("src/gy")
link_directories("src/si")
link_directories("src/lua")
link_directories("src/chipmunk")
link_directories("src/raku")

add_subdirectory("src/gy")
add_subdirectory("src/si")
add_subdirectory("src/lua")
add_subdirectory("src/chipmunk")
add_subdirectory("src/raku")
add_subdirectory("src/eruta")


enable_testing()
# Let ctest run valgrind
set(CMAKE_CTEST_OPTIONS --output-on-failure)
set(CTEST_OUTPUT_ON_FAILURE TRUE)
set(CTEST_MEMORYCHECK_COMMAND:FILEPATH "/usr/bin/valgrind")
add_subdirectory("test/si")
add_subdirectory("test/gy")
add_subdirectory("test/raku")
add_subdirectory("test/eruta")









