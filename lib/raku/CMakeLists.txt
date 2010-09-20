cmake_minimum_required(VERSION 2.6)
if(CMAKE_VERSION VERSION_GREATER "2.8.0")
  cmake_policy(SET CMP0012 OLD)
endif()  
# set(CMAKE_BUILT_TYPE Debug)
# below is for profiling.
#set(CMAKE_C_FLAGS "-Wall -g -pg -O")
# -Wall
set(CMAKE_C_FLAGS "-g -O")

# include_directories()
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
#find_package(FFI REQUIRED)
#INCLUDE_DIRECTORIES(${FFI_INCLUDE_DIR})
set(LIBS ${LIBS} m)

# configure_file("config.in.h" "config.h")

# set(LIBS ${LIBS} pthread m)
# Real Time enabled C Utility Library
set(LIBNAME raku)
set(${SRC_SUBDIR} ${CMAKE_SOURCE_DIR}/src/)
set(CFILES string.c)
add_library(${LIBNAME}_static STATIC ${CFILES})
add_library(${LIBNAME} SHARED ${CFILES})
target_link_libraries(raku ${LIBS})
set(TEST_LIBS ${LIBS} ${LIBNAME})
set(TEST_NAME raku_test)
add_executable(${TEST_NAME} ${SRC_SUBDIR}${TEST_NAME}.c)
target_link_libraries(${TEST_NAME} ${TEST_LIBS})
enable_testing()
set(TEST_EXE ${EXECUTABLE_OUTPUT_PATH}${TEST_NAME}) 
add_test(${TEST_NAME} ${TEST_EXE})
# Make sure that ctest shows the test error messages if there are any.
set(CMAKE_CTEST_OPTIONS --output-on-failure)
# Make check compiles the tests and runs them immediately.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} ${CMAKE_CTEST_OPTIONS} DEPENDS ${TEST_EXE})



