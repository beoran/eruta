file(GLOB RE_C_FILES "*.c")


add_library(re_static STATIC ${RE_C_FILES})
add_library(re SHARED ${RE_C_FILES})


