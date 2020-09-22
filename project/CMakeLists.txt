cmake_minimum_required(VERSION 3.10)
project("mvclmandelbrot")

include_directories(./source
  "/c/Program Files (x86)/AMD APP SDK/3.0/include"
)
link_directories("/c/Program Files (x86)/AMD APP SDK/3.0/lib/x86_64")


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

if (CMAKE_SYSTEM_NAME STREQUAL "MSYS")
    #
    # Windows
    #
    
    include_directories("/c/Program Files (x86)/AMD APP SDK/3.0/include")
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


