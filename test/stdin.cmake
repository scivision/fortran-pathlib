cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED exe)
  message(FATAL_ERROR "Please specify the executable to run -Dexe=<exe>")
endif()

if(NOT DEFINED regex)
  message(FATAL_ERROR "Please specify the regex to match -Dregex=<regex>")
endif()

if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/repl_cpp.txt)
  message(FATAL_ERROR "repl_cpp.txt does not exist")
endif()

execute_process(COMMAND ${exe}
INPUT_FILE ${CMAKE_CURRENT_LIST_DIR}/repl_cpp.txt
OUTPUT_VARIABLE out
OUTPUT_STRIP_TRAILING_WHITESPACE
RESULT_VARIABLE res
)

if(NOT res EQUAL 0)
  message(FATAL_ERROR "Error running ${exe}")
endif()

if(NOT "${out}" MATCHES "${regex}")
  message(FATAL_ERROR "${out} does not match ${regex}")
endif()
