cmake_minimum_required(VERSION 2.8)
project(Abstraction)

# -std=c++11
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# OpenCv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Add Sources
set(SOURCE_FILES
    SortedArray.h SortedArray.cpp
    Pixel.h Pixel.cpp
    Abstractor.h
    main.cpp
)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

