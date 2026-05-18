# BetrockViewer – Nintendo Switch Port Notes

## What was changed and why

### New files

| File | Purpose |
|------|---------|
| `CMakeLists_Switch.txt` | devkitpro/libnx CMake build system (separate from the Linux CMakeLists.txt) |
| `src/platform/nx_main.cpp` | Replaces `src/main.cpp` – EGL context, libnx main loop, Switch-specific paths |
| `src/platform/nx_imgui_nx.h/.cpp` | ImGui platform backend (replaces `imgui_impl_glfw`) – maps D-pad/A/B to ImGui navigation |
| `src/render/camera_nx.h` | Camera header without `GLFWwindow*` dependency |
| `src/render/camera_nx.cpp` | Camera using libnx `PadState` (left stick = move, right stick = look) |
| `src/render/sky_nx.cpp` | Sky draw that uploads `topColor`/`horizonColor`/`bottomColor` uniforms (required for GLSL ES) |
| `src/external/shaders_nx/` | GLSL ES 3.00 versions of all shaders |

### Why each change was necessary

#### GLFW removed → libnx EGL
GLFW does not target Nintendo Switch.  The Switch uses NVidia Tegra X1 with
OpenGL ES exposed via EGL.  `nx_main.cpp` calls `eglGetDisplay` /
`eglCreateWindowSurface(nwindowGetDefault())` instead of `glfwInit` /
`glfwCreateWindow`.

#### GLAD loaded for GLES2 loader
`gladLoadGL()` (desktop) is replaced by `gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress)`.
The in-tree `glad.c` / `glad.h` already contain the GLES2 symbols because they
were generated with the multi-API option; no new glad generation is needed.

#### GLSL `#version 330 core` → `#version 300 es`
OpenGL ES 3.x uses GLSL ES.  Key differences applied to every shader:
- First line changed from `#version 330 core` to `#version 300 es`
- `precision highp float;` added after the version line (required by GLSL ES)
- Trailing `f` float suffix (e.g. `1.0f`) removed (not valid in GLSL ES)
- `sky.fsh`: uniform default-value initialisers (`uniform vec3 topColor = …`)
  removed — GLSL ES forbids them.  The values are uploaded from `sky_nx.cpp`
  instead.

#### Input: GLFW keyboard/mouse → libnx HID
`camera_nx.cpp` maps:
- Left  analogue stick → camera translation (forward / back / strafe)
- Right analogue stick → camera rotation (yaw / pitch)
- ZR / ZL             → fly up / fly down
- R / L (hold)        → double / half speed
- Minus               → reset speed
- Plus (in main loop) → quit

ImGui UI is navigated with D-pad + A/B via `nx_imgui_nx.cpp`.

#### Filesystem paths → `romfs:/`
The Switch has no writable directory under the executable path.  Assets
(shaders, models, textures, saves) are embedded in the NRO's romfs section.
`BetrockViewer::basePath()` is set to `"romfs:"` before anything reads files.
Screenshots are written to `sdmc:/`.

---

## Build prerequisites

Install devkitpro with the Switch packages:

```bash
# Arch / pacman-based (use the devkitPro pacman repository)
sudo dkp-pacman -S switch-dev switch-mesa switch-glm libdeflate

# fmt – build from source or use the devkitpro port if available
# GLM is header-only; the switch-glm package puts it in $DEVKITPRO/portlibs/switch/include
```

Required devkitpro packages:
- `devkitA64` (AArch64 toolchain)
- `libnx`
- `switch-mesa`   (GLES / EGL)
- `switch-glm`    (GLM headers)
- `switch-fmt`    (fmt library) or build fmt with the Switch toolchain

---

## Build

```bash
# Set up devkitpro environment (usually done by /etc/profile.d/devkit-env.sh)
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=$DEVKITPRO/devkitARM
export PATH=$DEVKITPRO/tools/bin:$PATH

# Configure
cmake \
  -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/cmake/Switch.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -B build_nx \
  -G "Unix Makefiles" \
  -f CMakeLists_Switch.txt

# Build
cmake --build build_nx

# The output is build_nx/BetrockViewer.nro
# Copy to your Switch SD card:  /switch/BetrockViewer/BetrockViewer.nro
```

---

## Controls

| Button | Action |
|--------|--------|
| Left stick | Move camera (forward / back / strafe) |
| Right stick | Rotate camera (yaw / pitch) |
| ZR | Fly up |
| ZL | Fly down |
| R (hold) | Double speed |
| L (hold) | Half speed |
| Minus | Reset speed |
| D-pad | Navigate ImGui menus |
| A | Confirm (ImGui) |
| B | Back / close (ImGui) |
| ZL + ZR | Take screenshot (saved to `sdmc:/BetrockViewer_N.png`) |
| Plus | Quit |

---

## Known limitations / TODO

- The chunk-building thread body in `nx_main.cpp` has a `// TODO: paste from main.cpp`
  placeholder.  Copy the full `buildChunks` loop from `src/main.cpp` into
  `src/platform/nx_main.cpp` to complete the port.
- World selection is done via ImGui text input; saves bundled in romfs are
  read-only.  To load user worlds, copy them to `sdmc:/BetrockViewer/saves/`
  and adjust the load path in `nx_main.cpp`.
- The thread model (one background chunk builder) works on Switch but
  `std::thread` stack size defaults may need tuning via libnx thread APIs for
  very large worlds.
- `imgui_impl_glfw` is completely replaced; ImGui mouse/keyboard text input
  does not work (no on-screen keyboard implemented yet).
- Fullscreen toggle removed (Switch is always fullscreen).
