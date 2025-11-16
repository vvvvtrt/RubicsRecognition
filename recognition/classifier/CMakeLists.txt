add_library(classifier classifier.cpp classifier.hpp)
set_property(TARGET classifier PROPERTY CXX_STANDARD 20)
target_include_directories(classifier PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
include_directories(${OpenCV_INCLUDE_DIRS})

install(TARGETS classifier)