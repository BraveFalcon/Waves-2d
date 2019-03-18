cmake_minimum_required(VERSION 3.12)
project(Waves)

set(CMAKE_CXX_STANDARD 17)

add_executable(Waves main.cpp reader.h)

target_link_libraries(Waves glfw GLEW GL SOIL)