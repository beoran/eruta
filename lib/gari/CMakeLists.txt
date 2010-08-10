cmake_minimum_required(VERSION 2.6)
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
# FindSDL
# FindSDL_image
# FindSDL_mixer
# FindSDL_net
# FindSDL_sound
# FindSDL_ttf
# Set include and lib dirs. 
include_directories(${SDL_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDL_LIBRARY} ${SDLTTF_LIBRARY} ${SDLIMAGE_LIBRARY})

# set(LIBS ${LIBS} pthread m)
# Real Time enabled C Utility Library
set(CFILES gari.c image.c font.c pixel.c draw.c color.c tile.c 
           layer.c map.c sprite.c event.c camera.c particle.c)
add_library(gari STATIC ${CFILES})
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

