cmake_minimum_required(VERSION 2.4)

enable_testing()

file(GLOB tests "*test.c")
foreach(test_source_fullpath ${tests})
  string(REGEX REPLACE "^.*/" "" test_source ${test_source_fullpath})
  string(REPLACE "test.c" "test" test_bin ${test_source})
  message(${test_source})
  message(${test_bin})
  add_executable(${test_bin} ${test_source})
  add_test(${test_bin} ${test_bin})
endforeach()
