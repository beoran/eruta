file(GLOB TEST_C_FILES "*.c")
foreach(TEST_C_FILE ${TEST_C_FILES})
  get_filename_component(TEST_C_EXE ${TEST_C_FILE} NAME_WE)
  add_executable(${TEST_C_EXE} ${TEST_C_FILE})
  target_link_libraries(${TEST_C_EXE} re)
  add_test(${TEST_C_EXE} ${TEST_C_EXE})
endforeach(TEST_C_FILE)


