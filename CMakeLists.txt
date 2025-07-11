cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0072 NEW)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Betrock VERSION 0.3.8)

find_package(fmt REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
include_directories(src/include)
add_compile_options(
  "-ggdb"
)

file(COPY "${CMAKE_SOURCE_DIR}/src/external/"
DESTINATION "${CMAKE_BINARY_DIR}")

# Configure the version header file
configure_file(${CMAKE_SOURCE_DIR}/src/version.h.in ${CMAKE_SOURCE_DIR}/src/version.h)

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
    src/render/sky.cpp

    src/world/world.cpp
    src/world/regionLoader.cpp
    src/world/region.cpp
    src/world/chunk.cpp
    src/world/blockProperties.cpp
    
    src/helper.cpp
    src/debug.cpp
    src/main.cpp

    src/include/glad.c
    src/include/imgui/imgui.cpp
    src/include/imgui/imgui_draw.cpp
    src/include/imgui/imgui_tables.cpp
    src/include/imgui/imgui_widgets.cpp
    src/include/imgui/backends/imgui_impl_glfw.cpp
    src/include/imgui/backends/imgui_impl_opengl3.cpp
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
	fmt::fmt
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
	fmt::fmt
        ${CMAKE_DL_LIBS}
    )
endif()

include(CPack)

# Set up CPack for AppImage
set(CPACK_PACKAGE_NAME "Betrock")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Betrock - Minecraft Beta 1.7.3 World Explorer")
set(CPACK_PACKAGE_VENDOR "Torben J. Virtmann")
set(CPACK_PACKAGE_CONTACT "letsplaytvirtmann@gmail.com")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# AppImage specific settings
set(CPACK_GENERATOR "AppImage")
set(CPACK_APPIMAGE_FILE_ASSOCIATIONS "*.betrock")
set(CPACK_PACKAGE_EXECUTABLES "Betrock;Betrock")

# Ensure dependencies are bundled
set(CPACK_APPIMAGE_USE_SYSTEM_LIBS FALSE)

# Install the Betrock executable
install(TARGETS Betrock DESTINATION bin)

# Install additional resources
install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/external/" DESTINATION bin)
