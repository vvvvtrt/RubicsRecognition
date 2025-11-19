add_library(gui display.cpp display.hpp)
set_property(TARGET gui PROPERTY CXX_STANDARD 20)

set(GLFW3_LIBRARY /opt/homebrew/lib/libglfw.dylib)

find_library(OpenGL_LIBRARY OpenGL)
target_link_libraries(gui PRIVATE ${OpenGL_LIBRARY})

target_include_directories(gui PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
  ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(gui 
    PRIVATE 
    detector            
    preprocessor
    classifier
    recognition
    ${OpenCV_LIBS}
    ${GLFW3_LIBRARY}
)

include_directories(${OpenCV_INCLUDE_DIRS})

install(TARGETS gui)