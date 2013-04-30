cmake_minimum_required(VERSION 2.8)
# Set policy: link_directories is relative to source dir
cmake_policy(SET CMP0015 NEW)


# Set the project name.
project(ERUTA C)

# Play with PCC
# SET(CMAKE_C_COMPILER   /usr/local/bin/pcc)
# Play with UCC
# SET(CMAKE_C_COMPILER   /usr/local/lib/ucc/ucc)

# tell CMake to search first in the cmake subdirectory for FIND_PACKAGE() or INCLUDE()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)

include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)


if(CMAKE_COMPILER_IS_GNUCC)
 set(COMPILER_GCC 1)
set(CMAKE_C_FLAGS "-W -Wall -Wno-unused -Wno-unknown-pragmas -g -std=gnu99 -ffast-math -fno-strict-aliasing -Wstrict-aliasing=1")
# always use gnu99, debugging, all warnings except unused and unknown pragmas.
# when compiling with gnu compiler.  
# Warn about alisasing because otherwise aliasing problems it may not be detected.
endif(CMAKE_COMPILER_IS_GNUCC)

# or for PCC
# set(CMAKE_C_FLAGS "-fpic -fPIC")  



find_package(Mruby REQUIRED)
find_package(Chipmunk REQUIRED)

# Finds Allegro using pkgconfig, so it must be configured correctly 
find_package(Allegro5 REQUIRED)
find_package(LibXml2 REQUIRED)



find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

# Set include and lib dirs. 
include_directories(${ALLEGRO_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${MRUBY_INCLUDE_DIR} ${CHIPMUNK_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR}) 
set(OBJC_LIBRARIES -lobjc)


set(ERUTA_LIBS ${LIBS} ${OBJC_LIBRARIES} ${ALLEGRO_LIBRARIES} ${PNG_LIBRARY} ${JPEG_LIBRARY}
${LUA_LIBRARIES} ${MRUBY_LIBRARIES} ${CHIPMUNK_LIBRARIES} ${LIBXML2_LIBRARIES})

include_directories("include")
# add_subdirectory("src")
include(ErutaFiles)
set_source_files_properties(${ERUTA_SRC_FILES} PROPERTIES LANGUAGE C)

# set_source_files_properties(${ERUTA_OBJC_SRC_FILES} PROPERTIES LANGUAGE C)
# src/BNObject.m 
# set_source_files_properties(BNObject.m PROPERTIES LANGUAGE C)




# eruta exe in the bin subdir (first one should work, but doesnt, hmmm...)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_library(ERUTA_OBJECTS OBJECT ${ERUTA_SRC_FILES})
add_executable(eruta $<TARGET_OBJECTS:ERUTA_OBJECTS> src/main.c)
target_link_libraries(eruta ${ERUTA_LIBS})

enable_testing()
# Let ctest run valgrind
# test exe in the test subdir (first one should work, but doesnt, hmmm...)
set(CMAKE_CTEST_OPTIONS --output-on-failure)
set(CTEST_OUTPUT_ON_FAILURE TRUE)
set(CTEST_MEMORYCHECK_COMMAND:FILEPATH "/usr/bin/valgrind")
add_subdirectory("test")









