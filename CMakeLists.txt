cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)

set(FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

project(stitching)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(stitching_detailed stitching_detailed.cpp)
target_link_libraries(stitching_detailed ${OpenCV_LIBS})

add_executable(stitcher stitcher.cpp)
target_link_libraries(stitcher ${OpenCV_LIBS})

add_executable(markers markers.cpp)
target_link_libraries(markers ${OpenCV_LIBS})

SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)

