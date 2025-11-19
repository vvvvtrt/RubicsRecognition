add_library(detector detector.cpp detector.hpp)
set_property(TARGET detector PROPERTY CXX_STANDARD 20)
target_include_directories(detector PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
include_directories(${OpenCV_INCLUDE_DIRS})

install(TARGETS detector)