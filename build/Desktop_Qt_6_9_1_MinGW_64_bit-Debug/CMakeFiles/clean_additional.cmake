# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\stocks1_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\stocks1_autogen.dir\\ParseCache.txt"
  "stocks1_autogen"
  )
endif()
