# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Exp_Cal_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Exp_Cal_autogen.dir\\ParseCache.txt"
  "Exp_Cal_autogen"
  )
endif()
