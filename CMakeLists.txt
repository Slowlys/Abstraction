cmake_minimum_required(VERSION 2.8)
project(Abstraction)

# -std=c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

# OpenCv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Add Sources
set(SOURCE_FILES
    Pixel.hpp Pixel.cpp
    Abstractor.hpp Abstractor.cpp
    main.cpp
)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
