cmake_minimum_required(VERSION 3.28.3)
project(Betrock VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)
set (CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

add_executable(
    Betrock
    src/nbt/nbt.cpp
    src/nbt/nbttag.cpp
    src/render/shader.cpp
    src/render/vao.cpp
    src/render/vbo.cpp
    src/render/ebo.cpp
    src/render/texture.cpp
    src/render/camera.cpp
    src/world/regionLoader.cpp
    src/world/region.cpp
    src/world/chunk.cpp
    src/world/block.cpp
    src/glad.c
    src/helper.cpp
    src/main.cpp
)
target_link_libraries(
    Betrock
    glfw
    OpenGL::GL
    deflate
    stb
)