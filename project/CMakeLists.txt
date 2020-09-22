cmake_minimum_required(VERSION 3.10)
project("mvclmandelbrot")

message("System: ${CMAKE_SYSTEM_NAME}")

include_directories(./source)

#
# AMD SPP SDK (Window only):
#

if (CMAKE_SYSTEM_NAME STREQUAL "MSYS")
    include_directories("/c/Program Files (x86)/AMD APP SDK/3.0/include")
    link_directories("/c/Program Files (x86)/AMD APP SDK/3.0/lib/x86_64")
endif()

#
# OPENCL
#

find_package(OPENCL REQUIRED)
if (OPENCL_FOUND)
    message("found OPENCL")
    include_directories($(OPENCL_INCLUDE_DIRS))
    link_libraries(${OPENCL_LIBRARIES})
endif()

#
#
#

add_executable(mvclmandelbrot
    source/main.cpp
    source/WriteTga.cpp
    source/NStringTool.cpp
    )

#
#
#

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(mvclmandelbrot "-framework OpenCL")
else()
    target_link_libraries(mvclmandelbrot OpenCL)
endif()


