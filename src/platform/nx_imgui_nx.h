#pragma once
/**
 * src/platform/nx_imgui_nx.h
 *
 * Minimal ImGui platform backend for Nintendo Switch (libnx).
 * Replaces imgui_impl_glfw entirely: no keyboard / mouse events,
 * just passes controller button presses as ImGui key events so menus
 * can be navigated with the D-pad.
 */

#include <switch.h>
#include "../include/imgui/imgui.h"

IMGUI_IMPL_API bool ImGui_ImplNX_Init();
IMGUI_IMPL_API void ImGui_ImplNX_Shutdown();
IMGUI_IMPL_API void ImGui_ImplNX_NewFrame(const PadState& pad);
