file(GLOB SI_C_FILES "*.c;*_*.c")


add_library(si_static STATIC ${SI_C_FILES})
add_library(si SHARED ${SI_C_FILES})


