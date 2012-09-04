file(GLOB TEST_C_FILES "test_*.c")
foreach(TEST_C_FILE ${TEST_C_FILES})
  get_filename_component(TEST_C_EXE ${TEST_C_FILE} NAME_WE)
  add_executable(${TEST_C_EXE} $<TARGET_OBJECTS:ERUTA_OBJECTS> ${TEST_C_FILE})
  target_link_libraries(${TEST_C_EXE} ${ERUTA_LIBS})
  add_test(${TEST_C_EXE} ${CMAKE_BINARY_DIR}/bin/${TEST_C_EXE})
  # Make sure that ctest shows the test error messages if there are any.
  set(CMAKE_CTEST_OPTIONS --output-on-failure)
endforeach(TEST_C_FILE)

# Make check compiles the tests and runs them immediately.
# add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} ${CMAKE_CTEST_OPTIONS} DEPENDS ${TEST_EXE})



