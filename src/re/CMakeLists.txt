file(GLOB C_FILES "*.c")


add_library(re_static STATIC ${C_FILES})
add_library(re SHARED ${C_FILES})


