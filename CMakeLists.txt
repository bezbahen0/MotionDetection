cmake_minimum_required(VERSION 3.6.0)

project(MovementDetector LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_compile_options(
    # -Wall
)

file(GLOB HEADERS "include/*.hpp")
file(GLOB SOURCES  "src/*.cpp")

if(OpenCV_FOUND)
    include_directories( ${OpenCV_INCLUDE_DIRS} )
    include_directories( ${PROJECT_SOURCE_DIR} )
    add_executable(  MovementDetector ${SOURCES} ${HEADERS} )
    target_link_libraries( MovementDetector ${OpenCV_LIBS} )
else()
    message(STATUS "OpenCV not found...")
endif()
