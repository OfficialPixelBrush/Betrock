cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0072 NEW)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(LegacyWorldViewer VERSION 0.4.3)

find_package(fmt REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_library(glad src/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/src/include/)

include_directories(
    src/include
    src/include/stb
    src/include/imgui
    src/include/glad
)

# Debug info in all builds; strip for release via CMAKE_BUILD_TYPE=Release
add_compile_options("-ggdb")

# Copy runtime assets next to the binary during builds so dev runs work
# (the AppImage build copies them to the proper install tree instead)
file(COPY "${CMAKE_SOURCE_DIR}/src/external/"
     DESTINATION "${CMAKE_BINARY_DIR}")

# Configure the version header file
configure_file(${CMAKE_SOURCE_DIR}/src/version.h.in ${CMAKE_SOURCE_DIR}/src/version.h)

add_executable(
    LegacyWorldViewer
    src/model/chunkBuilder.cpp
    src/model/model.cpp

    src/render/shader.cpp
    src/render/vao.cpp
    src/render/vbo.cpp
    src/render/ebo.cpp
    src/render/texture.cpp
    src/render/camera.cpp
    src/render/mesh.cpp
    src/render/sky.cpp

    src/world/world.cpp
    src/world/loaders/regionLoader.cpp
    src/world/loaders/indevLoader.cpp
    src/world/loaders/alphaLoader.cpp
    src/world/loaders/worldLoader.cpp
    src/world/region.cpp
    src/world/chunk.cpp
    src/world/blockProperties.cpp

    src/helper.cpp
    src/debug.cpp
    src/main.cpp

    src/include/stb_include.cpp
    src/include/imgui/imgui.cpp
    src/include/imgui/imgui_draw.cpp
    src/include/imgui/imgui_tables.cpp
    src/include/imgui/imgui_widgets.cpp
    src/include/imgui/backends/imgui_impl_glfw.cpp
    src/include/imgui/backends/imgui_impl_opengl3.cpp
)

# TODO: Get Windows Target working
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
    set(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
    set(CMAKE_RANLIB i686-w64-mingw32-ranlib)
    target_link_libraries(
        LegacyWorldViewer
        glad
        gdi32
        opengl32
        deflate
        fmt::fmt
    )
else()
    # AddressSanitizer is useful for development but must NOT be included in
    # AppImage / release builds – libasan is not portable and the AppImage
    # won't run on systems where the exact libasan version is absent.
    # Enable it explicitly with -DUSE_ASAN=ON if you want it.
    option(USE_ASAN "Build with AddressSanitizer" OFF)
    if(USE_ASAN)
        message(STATUS "AddressSanitizer enabled")
        target_compile_options(LegacyWorldViewer PRIVATE -fsanitize=address -g)
        target_link_options(LegacyWorldViewer PRIVATE -fsanitize=address)
    endif()

    target_link_libraries(
        LegacyWorldViewer
        glad
        glfw
        OpenGL::GL
        deflate
        fmt::fmt
        ${CMAKE_DL_LIBS}
    )
endif()

# ── Install layout ────────────────────────────────────────────────────────────
# The install tree mirrors what linuxdeploy expects inside an AppDir:
#
#   usr/bin/LegacyWorldViewer
#   usr/share/LegacyWorldViewer/shaders/
#   usr/share/LegacyWorldViewer/textures/
#   usr/share/LegacyWorldViewer/models/
#   usr/share/LegacyWorldViewer/saves/
#   usr/share/applications/LegacyWorldViewer.desktop
#   usr/share/icons/hicolor/256x256/apps/LegacyWorldViewer.png
#
# The executable resolves $APPDIR/usr/share/LegacyWorldViewer at runtime when running
# as an AppImage (see main.cpp basePath logic).

install(TARGETS LegacyWorldViewer DESTINATION bin)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/external/shaders"  DESTINATION share/LegacyWorldViewer)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/external/textures" DESTINATION share/LegacyWorldViewer)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/external/models"   DESTINATION share/LegacyWorldViewer)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/external/saves"    DESTINATION share/LegacyWorldViewer)

install(FILES "${CMAKE_SOURCE_DIR}/LegacyWorldViewer.desktop"
        DESTINATION share/applications)
install(FILES "${CMAKE_SOURCE_DIR}/LegacyWorldViewer.png"
        DESTINATION share/icons/hicolor/256x256/apps
        RENAME LegacyWorldViewer.png)

# ── CPack packaging ───────────────────────────────────────────────────────────
set(CPACK_PACKAGE_NAME "LegacyWorldViewer")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "Pixel Brush")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LegacyWorldViewer - Legacy Minecraft World Explorer")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "LegacyWorldViewer")

# For TGZ: just pack the install tree
set(CPACK_GENERATOR "TGZ")
set(CPACK_STRIP_FILES TRUE)

include(CPack)
