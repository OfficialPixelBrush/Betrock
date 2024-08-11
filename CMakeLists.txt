cmake_minimum_required(VERSION 3.28.3)
project(Betrock VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(
    Betrock
    src/config.h
    src/main.cpp
    src/region.cpp
    src/regionLoader.cpp
    src/nbt.cpp
    src/glad.c
)
target_link_libraries(
    Betrock
    glfw
    OpenGL::GL
    deflate
)