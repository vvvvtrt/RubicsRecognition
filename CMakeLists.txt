cmake_minimum_required(VERSION 3.16)
project(RubiksCubeRecognition)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR /opt/homebrew/opt/opencv/share/opencv4)

find_package(OpenCV REQUIRED)

add_subdirectory(recognition)
add_subdirectory(src)
