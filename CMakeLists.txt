cmake_minimum_required(VERSION 3.28.3)
cmake_policy(SET CMP0072 NEW)
set (CMAKE_CXX_STANDARD 20)
project(Betrock VERSION 0.1.0)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
add_compile_options(
  "-ggdb"
)

add_executable(
    Betrock
    src/model/chunkBuilder.cpp
    src/model/model.cpp

    src/nbt/nbt.cpp
    src/nbt/nbttag.cpp

    src/render/shader.cpp
    src/render/vao.cpp
    src/render/vbo.cpp
    src/render/ebo.cpp
    src/render/texture.cpp
    src/render/camera.cpp
    src/render/mesh.cpp

    src/world/world.cpp
    src/world/regionLoader.cpp
    src/world/region.cpp
    src/world/chunk.cpp
    src/world/block.cpp

    src/external/glad.c
    src/helper.cpp
    src/main.cpp
)

set(CMAKE_SYSTEM_NAME Linux)

# TODO: Get Windows Target working
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
    set(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
    set(CMAKE_RANLIB i686-w64-mingw32-ranlib)
    target_link_libraries(
        Betrock
        gdi32
        opengl32
        deflate
        stb
        imgui
    )
else()
    # Apply AddressSanitizer to all build types
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
    target_link_libraries(
        Betrock
        glfw
        OpenGL::GL
        deflate
        stb
        imgui
    )
endif()