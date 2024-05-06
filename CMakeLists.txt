cmake_minimum_required(VERSION 3.23)
project(Tetris)

set(CMAKE_CXX_STANDARD 14)

add_executable(Tetris main.cpp src/glad.c shader.cpp)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(Tetris ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")