cmake_minimum_required(VERSION 3.0)

project(exmpl)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(exmpl exmpl.cpp)
target_link_libraries(exmpl ${OpenCV_LIBS})