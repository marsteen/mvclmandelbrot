cmake_minimum_required(VERSION 3.10)
project("mvclmandelbrot")

include_directories(./source)
message("System: ${CMAKE_SYSTEM_NAME}")

#
# OPENCL
#
find_package(OPENCL REQUIRED)
if (OPENCL_FOUND)
    message("found OPENCL")
    include_directories($(OPENCL_INCLUDE_DIRS))
    link_libraries(${OPENCL_LIBRARIES})
endif()

add_executable(mvclmandelbrot
    source/main.cpp
    source/WriteTga.cpp
    source/NStringTool.cpp
    )


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

#set_target_properties(mvclmandelbrot PROPERTIES
#                      RUNTIME_OUTPUT_DIRECTORY_DEBUG .
#                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ..
#)

if (CMAKE_SYSTEM_NAME STREQUAL "MSYS")
    #
    # Windows
    #
    target_link_libraries(mvclmandelbrot
        OpenCL)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    #
    # Linux
    #
    target_link_libraries(mvclmandelbrot
        OpenCL)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    #
    # Mac OS
    #
    target_link_libraries(mvclmandelbrot
        "-framework OpenCL")
endif()


