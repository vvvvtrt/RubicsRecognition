add_library(preprocessor preprocessor.cpp preprocessor.hpp)
set_property(TARGET preprocessor PROPERTY CXX_STANDARD 20)
target_include_directories(preprocessor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
include_directories(${OpenCV_INCLUDE_DIRS})
install(TARGETS preprocessor)