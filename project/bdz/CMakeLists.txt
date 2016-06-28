cmake_minimum_required(VERSION 3.0)

project(bdz)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(bdz bdz.cpp)
target_link_libraries(bdz ${OpenCV_LIBS})