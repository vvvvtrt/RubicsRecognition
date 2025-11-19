file(GLOB SRC_SOURCES *.cpp)

add_executable(RubiksCubeApp ${SRC_SOURCES})

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

target_include_directories(RubiksCubeApp PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(RubiksCubeApp PRIVATE ${OpenCV_LIBS})
target_include_directories(RubiksCubeApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)


set(GLFW3_LIBRARY /opt/homebrew/lib/libglfw.dylib)

find_library(OpenGL_LIBRARY OpenGL)
target_link_libraries(RubiksCubeApp PRIVATE ${OpenGL_LIBRARY})

add_definitions(-DGL_SILENCE_DEPRECATION)



target_link_libraries(RubiksCubeApp 
    PRIVATE 
    detector            
    preprocessor
    classifier
    recognition
    ${OpenCV_LIBS}
    ${GLFW3_LIBRARY}
)

include_directories(${OpenCV_INCLUDE_DIRS})
