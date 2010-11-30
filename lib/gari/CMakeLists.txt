cmake_minimum_required(VERSION 2.6)
if(CMAKE_VERSION VERSION_GREATER "2.8.0")
  cmake_policy(SET CMP0012 OLD)
endif()  
# set(CMAKE_BUILT_TYPE Debug)
# below is for profiling.
#set(CMAKE_C_FLAGS "-Wall -g -pg -O")
set(CMAKE_C_FLAGS "-Wall -g -O")

# Find SDL
set(SDL_BUILDING_LIBRARY 1)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(Ruby 1.9 REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
find_package(Lua REQUIRED)

# FindSDL
# FindSDL_image
# FindSDL_mixer
# FindSDL_net
# FindSDL_sound
# FindSDL_ttf
# Set include and lib dirs. 

message("Ruby: >" ${RUBY_FOUND} "<")
if(${RUBY_FOUND})
else()
set(RUBY_INCLUDE_PREFIX /usr/local/include/ruby-1.9.1/)
set(RUBY_INCLUDE_DIR1 ${RUBY_INCLUDE_PREFIX})
set(RUBY_INCLUDE_DIR2 ${RUBY_INCLUDE_PREFIX}/i686-linux)
set(RUBY_INCLUDE_DIR3 ${RUBY_INCLUDE_PREFIX}/backward)
set(RUBY_INCLUDE_DIRS 
   ${RUBY_INCLUDE_DIR1} ${RUBY_INCLUDE_DIR2} ${RUBY_INCLUDE_DIR3})
set(RUBY_LIBRARY -L/usr/local/lib ruby)
endif()
message("Ruby: " ${RUBY_INCLUDE_DIRS})
message("Lua: " ${LUA_LIBRARIES})

include_directories(${SDL_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR}
${RUBY_INCLUDE_DIRS} ${LUA_INCLUDE_DIRS})

set(LIBS ${LIBS} ${SDL_LIBRARY} ${SDLTTF_LIBRARY} 
${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${RUBY_LIBRARY}
${LUA_LIBRARIES})

# configure_file("config.in.h" "config.h")

# set(LIBS ${LIBS} pthread m)
# Real Time enabled C Utility Library
set(CFILES gari.c audio.c image.c font.c pixel.c draw.c color.c tile.c 
           layer.c map.c script.c sprite.c event.c camera.c particle.c
           style.c ruby.c)
add_library(gari_static STATIC ${CFILES})
add_library(gari SHARED ${CFILES})
target_link_libraries(gari ${LIBS})
set(TEST_LIBS ${LIBS} gari) 
add_executable(gari_test gari_test.c)
target_link_libraries(gari_test ${TEST_LIBS})
enable_testing()
set(TEST_EXE ${EXECUTABLE_OUTPUT_PATH}gari_test) 
add_test(gari_test ${TEST_EXE})
# Make sure that ctest shows the test error messages if there are any.
set(CMAKE_CTEST_OPTIONS --output-on-failure)
# Make check compiles the tests and runs them immediately.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} ${CMAKE_CTEST_OPTIONS} DEPENDS ${TEST_EXE})



add_executable(gari_spec gari_spec.c)
target_link_libraries(gari_spec ${TEST_LIBS})
set(SPEC_EXE ${EXECUTABLE_OUTPUT_PATH}gari_spec)
# Make spec compiles the specs and runs them immediately.
add_custom_target(spec COMMAND ${SPEC_EXE} DEPENDS ${SPEC_EXE})

