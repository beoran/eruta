cmake_minimum_required(VERSION 2.6)
if(CMAKE_VERSION VERSION_GREATER "2.8.0")
  cmake_policy(SET CMP0012 OLD)
endif()  
# set(CMAKE_BUILT_TYPE Debug)
# below is for profiling.
#set(CMAKE_C_FLAGS "-Wall -g -pg -O")
set(CMAKE_C_FLAGS "-Wall -g -O")

# include_directories()
# set(LIBS ${LIBS})

# configure_file("config.in.h" "config.h")

# set(LIBS ${LIBS} pthread m)
# Real Time enabled C Utility Library
set(LIBNAME nori)
set(CFILES row.c item.c run.c vm.c)
add_library(${LIBNAME}_static STATIC ${CFILES})
add_library(${LIBNAME} SHARED ${CFILES})
target_link_libraries(nori ${LIBS})
set(TEST_LIBS ${LIBS} ${LIBNAME})
add_executable(nori_test nori_test.c)
target_link_libraries(nori_test ${TEST_LIBS})
enable_testing()
set(TEST_EXE ${EXECUTABLE_OUTPUT_PATH}nori_test) 
add_test(nori_test ${TEST_EXE})
# Make sure that ctest shows the test error messages if there are any.
set(CMAKE_CTEST_OPTIONS --output-on-failure)
# Make check compiles the tests and runs them immediately.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} ${CMAKE_CTEST_OPTIONS} DEPENDS ${TEST_EXE})



