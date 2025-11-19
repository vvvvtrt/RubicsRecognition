add_library(recognition
    recognition.cpp
    recognition.hpp
)

# Добавляем текущую директорию в include path
target_include_directories(recognition PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(recognition
    PUBLIC classifier detector preprocessor ${OpenCV_LIBS}
)

set_property(TARGET recognition PROPERTY CXX_STANDARD 20)
install(TARGETS recognition)
