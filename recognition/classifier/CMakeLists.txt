add_library(classifer classifer.cpp classifer.hpp)
set_property(TARGET classifer PROPERTY CXX_STANDARD 20)
target_include_directories(classifer PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
include_directories(${OpenCV_INCLUDE_DIRS})

install(TARGETS classifer)