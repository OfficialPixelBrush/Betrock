cmake_minimum_required(VERSION 3.28.3)
project(Betrock VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)
set (CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

add_executable(
    Betrock
    src/config.h
    src/main.cpp
    src/helper.cpp
    src/nbt.cpp
    src/nbttag.cpp
    src/world/regionLoader.cpp
    src/world/region.cpp
    src/world/chunk.cpp
    src/glad.c
)
target_link_libraries(
    Betrock
    glfw
    OpenGL::GL
    deflate
)