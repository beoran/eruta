file(GLOB RAKU_C_FILES "*.c")


add_library(raku_static STATIC ${RAKU_C_FILES})
add_library(raku SHARED ${RAKU_C_FILES})


