file(GLOB chipmunk_source_files "*.c" "constraints/*.c")
file(GLOB chipmunk_public_header "${ERUTA_SOURCE_DIR}/include/chipmunk/*.h")
file(GLOB chipmunk_constraint_header "${ERUTA_SOURCE_DIR}/include/chipmunk/constraints/*.h")

include_directories(${ERUTA_SOURCE_DIR}/include/chipmunk)
add_library(chipmunk SHARED  ${chipmunk_source_files})
# set the lib's version number?
# set_target_properties(chipmunk PROPERTIES VERSION 6.0.0)
add_library(chipmunk_static STATIC  ${chipmunk_source_files})
# Sets chipmunk_static to output "libchipmunk.a" not "libchipmunk_static.a"
# set_target_properties(chipmunk_static PROPERTIES OUTPUT_NAME chipmunk)
