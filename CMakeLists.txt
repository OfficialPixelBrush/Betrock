cmake_minimum_required(VERSION 3.16.0)

# ── devkitpro / libnx toolchain ───────────────────────────────────────────────
# Build with:
#   cmake -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/cmake/Switch.cmake \
#         -B build_nx -G"Unix Makefiles" -f CMakeLists_Switch.txt
#   cmake --build build_nx
# ──────────────────────────────────────────────────────────────────────────────

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(BetrockViewer VERSION 0.4.3)

# devkitpro cmake helpers expose nx_add_nro_target, find_package(libnx) etc.
find_package(PkgConfig REQUIRED)

# ── Switch OpenGL via glad + EGL (provided by devkitpro mesa/deko port) ───────
# devkitpro distributes switch-mesa which gives us EGL + OpenGL ES 3.2
pkg_check_modules(MESA REQUIRED
    egl
    glapi
    glesv2
)
#pkg_check_modules(SWITCH_EGL  REQUIRED switch-egl)

# ── GLM (header-only) ─────────────────────────────────────────────────────────
find_path(GLM_INCLUDE_DIR glm/glm.hpp)

# ── libdeflate ────────────────────────────────────────────────────────────────
pkg_check_modules(DEFLATE REQUIRED libdeflate)

# ── fmt ───────────────────────────────────────────────────────────────────────
include_directories($ENV{DEVKITPRO}/portlibs/switch/include)

# ── glad (we compile it ourselves; already in-tree) ───────────────────────────
add_library(glad src/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/src/include/)

# ── Include directories ───────────────────────────────────────────────────────
include_directories(
    src/include
    src/include/stb
    src/include/imgui
    ${SWITCH_MESA_INCLUDE_DIRS}
    ${SWITCH_EGL_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)
include_directories($ENV{DEVKITPRO}/libnx/include)

# ── Configure version header ──────────────────────────────────────────────────
configure_file(${CMAKE_SOURCE_DIR}/src/version.h.in ${CMAKE_SOURCE_DIR}/src/version.h)

# ── Copy runtime assets into the build directory ──────────────────────────────
# Copy everything except desktop shaders first, then overlay the GLSL ES shaders
file(COPY "${CMAKE_SOURCE_DIR}/src/external/textures" DESTINATION "${CMAKE_BINARY_DIR}/romfs")
file(COPY "${CMAKE_SOURCE_DIR}/src/external/models"   DESTINATION "${CMAKE_BINARY_DIR}/romfs")
file(COPY "${CMAKE_SOURCE_DIR}/src/external/saves"    DESTINATION "${CMAKE_BINARY_DIR}/romfs")
# GLSL ES 3.00 shaders go in romfs/shaders/ (replace desktop #version 330 shaders)
file(COPY "${CMAKE_SOURCE_DIR}/src/external/shaders_nx/"
     DESTINATION "${CMAKE_BINARY_DIR}/romfs/shaders")

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(BetrockViewer
    # ── Platform-specific entry point (replaces GLFW with libnx EGL) ──────────
    src/nx_main.cpp

    src/model/chunkBuilder.cpp
    src/model/model.cpp

    src/render/shader.cpp
    src/render/vao.cpp
    src/render/vbo.cpp
    src/render/ebo.cpp
    src/render/texture.cpp
    src/render/camera_nx.cpp      # Switch-specific camera (libnx HID)
    src/render/mesh.cpp
    src/render/sky_nx.cpp       # GLSL-ES-compatible sky (uploads colour uniforms)

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

    src/include/stb_include.cpp
    src/include/imgui/imgui.cpp
    src/include/imgui/imgui_draw.cpp
    src/include/imgui/imgui_tables.cpp
    src/include/imgui/imgui_widgets.cpp
    # ImGui backend: OpenGL3 (GLES-compatible when IMGUI_IMPL_OPENGL_ES3 is defined)
    src/include/imgui/backends/imgui_impl_opengl3.cpp
    # No imgui_impl_glfw — replaced by our own nx_imgui_nx.cpp stub
    src/platform/nx_imgui_nx.cpp
)

target_include_directories(BetrockViewer PRIVATE ${MESA_INCLUDE_DIRS})
target_link_libraries(BetrockViewer PRIVATE
    glad
    ${MESA_LIBRARIES}
    ${DEFLATE_LIBRARIES}
)

# ── Compiler definitions ──────────────────────────────────────────────────────
target_compile_definitions(BetrockViewer PRIVATE
    # Tell ImGui's OpenGL3 backend to use GLES 3.0 instead of desktop GL
    IMGUI_IMPL_OPENGL_ES3
    IMGUI_DISABLE_WIN32_FUNCTIONS
    IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS
    # Our own guard so shared source files can conditionally compile
    PLATFORM_SWITCH
)

# ── devkitpro NRO packaging ───────────────────────────────────────────────────
# Produces BetrockViewer.nro with embedded romfs from build/romfs/
if(COMMAND nx_add_nro_target)
    nx_add_nro_target(BetrockViewer
        NAME    "BetrockViewer"
        AUTHOR  "Pixel Brush"
        VERSION "${PROJECT_VERSION}"
        ROMFS   "${CMAKE_BINARY_DIR}/romfs"
    )
endif()
