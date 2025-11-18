file(GLOB SRC_SOURCES *.cpp)

add_executable(RubiksCubeApp ${SRC_SOURCES})

target_include_directories(RubiksCubeApp PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(RubiksCubeApp PRIVATE ${OpenCV_LIBS})
target_include_directories(RubiksCubeApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(RubiksCubeApp 
    PRIVATE 
    detector            
    preprocessor
    classifier
    ${OpenCV_LIBS}
)

include_directories(${OpenCV_INCLUDE_DIRS})
