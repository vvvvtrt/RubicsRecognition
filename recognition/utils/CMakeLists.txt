add_library(utils circut.cpp circut.hpp)
set_property(TARGET utils PROPERTY CXX_STANDARD 20)
target_include_directories(utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
include_directories(${OpenCV_INCLUDE_DIRS})

install(TARGETS utils)