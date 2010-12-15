cmake_minimum_required(VERSION 2.4)

# Anything in this dir which ends with "test.c" is considered a test.
message("-- Tests to run:")
file(GLOB tests "*test.c")
foreach(test_source_fullpath ${tests})
  string(REGEX REPLACE "^.*/" "" test_source ${test_source_fullpath})
  string(REPLACE "test.c" "test" test_bin ${test_source})
  message("     " ${test_source})
  add_executable(${test_bin} ${test_source})
  target_link_libraries(${test_bin} ffff)
  add_test(${test_bin} ${test_bin})
endforeach()
# Add an empty line after tests.
message("")
