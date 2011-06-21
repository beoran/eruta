file(GLOB ERUTA_C_FILES "*.c")
add_executable(eruta ${ERUTA_C_FILES})
target_link_libraries(eruta gari si)
