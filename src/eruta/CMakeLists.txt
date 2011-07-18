file(GLOB ERUTA_C_FILES "*.c")
set(ERUTALIB_C_FILES ${ERUTA_C_FILES})
list(REMOVE_ITEM ERUTALIB_C_FILES "eruta.c")
add_executable(eruta ${ERUTA_C_FILES})
add_library(eruta_static STATIC ${ERUTALIB_C_FILES})
target_link_libraries(eruta ${GY_LIBS} gy_static si_static m lua_static
chipmunk_static)
target_link_libraries(eruta_static ${GY_LIBS} gy_static si_static m
lua_static chipmunk_static eruta_static)
  
