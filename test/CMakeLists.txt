file(GLOB TEST_C_FILES "test_*.c")
foreach(TEST_C_FILE ${TEST_C_FILES})
  get_filename_component(TEST_C_EXE ${TEST_C_FILE} NAME_WE)
  add_executable(${TEST_C_EXE} $<TARGET_OBJECTS:ERUTA_OBJECTS> ${TEST_C_FILE})
  target_link_libraries(${TEST_C_EXE} ${ERUTA_LIBS})
  add_test(${TEST_C_EXE} ${CMAKE_BINARY_DIR}/bin/${TEST_C_EXE})
  # Make sure that ctest shows the test error messages if there are any.
  set(CMAKE_CTEST_OPTIONS --output-on-failure)
endforeach(TEST_C_FILE)

file(GLOB TEST_OBJC_FILES "test_*.m")
# Pretend they are C files, gcc will know what to do.
set_source_files_properties(${TEST_OBJC_FILES} PROPERTIES LANGUAGE C)

foreach(TEST_OBJC_FILE ${TEST_OBJC_FILES})    
  get_filename_component(TEST_OBJC_EXE ${TEST_OBJC_FILE} NAME_WE)
  add_executable(${TEST_OBJC_EXE} $<TARGET_OBJECTS:ERUTA_OBJECTS> ${TEST_OBJC_FILE})
  target_link_libraries(${TEST_OBJC_EXE} ${ERUTA_LIBS})
  add_test(${TEST_OBJC_EXE} ${CMAKE_BINARY_DIR}/bin/${TEST_OBJC_EXE})
  message(STATUS "For ${TEST_OBJC_FILE} : ${TEST_OBJC_EXE}")
  # Make sure that ctest shows the test error messages if there are any.
  set(CMAKE_CTEST_OPTIONS --output-on-failure)
endforeach(TEST_OBJC_FILE)


# Make check compiles the tests and runs them immediately.
# add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} ${CMAKE_CTEST_OPTIONS} DEPENDS ${TEST_EXE})



