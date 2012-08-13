cmake_minimum_required(VERSION 2.8)
# Set policy: link_directories is relative to source dir
cmake_policy(SET CMP0015 NEW)

set(CMAKE_C_FLAGS "-Wall -Wno-unused -Wno-unknown-pragmas -g -std=gnu99 -ffast-math")  
# always use gnu99, debugging, all warnings except unused and unknown pragmas.

# tell CMake to search first in the cmake subdirectory for FIND_PACKAGE() or INCLUDE()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)

# Find Allegro using pkgconfig, which means some manual coping of 
# Allegro's pkgconfig files to /usr/lib/pkgconfig
# find_package(PkgConfig)
# pkg_check_modules(ALLEGRO REQUIRED
#   allegro-5.0
#   allegro_image-5.0
#   allegro_font-5.0
#   allegro_primitives-5.0
#   allegro_ttf-5.0
#   allegro_audio-5.0  
#   allegro_dialog-5.0  
#   allegro_memfile-5.0  
#   allegro_acodec-5.0  
#   allegro_color-5.0  
#   allegro_main-5.0   
#   allegro_physfs-5.0   
# ) 
# 
# MESSAGE (${ALLEGRO})
find_package(Lua52 REQUIRED)
find_package(Allegro50 REQUIRED)

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

# Set include and lib dirs. 
include_directories(${ALLEGRO50_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})

set(ERUTA_LIBS ${LIBS} ${ALLEGRO50_LIBRARY} ${PNG_LIBRARY} ${JPEG_LIBRARY})

include_directories("include")
add_subdirectory("src")


enable_testing()
# Let ctest run valgrind
set(CMAKE_CTEST_OPTIONS --output-on-failure)
set(CTEST_OUTPUT_ON_FAILURE TRUE)
set(CTEST_MEMORYCHECK_COMMAND:FILEPATH "/usr/bin/valgrind")
add_subdirectory("test")









