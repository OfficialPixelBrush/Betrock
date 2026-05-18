/**
 * src/platform/nx_imgui_nx.cpp
 *
 * Minimal Switch ImGui platform backend.
 *
 * D-pad / A / B are mapped to ImGui navigation so menus work without a
 * keyboard or mouse.  Stick values are not forwarded to ImGui (the camera
 * consumes them directly in nx_main.cpp / camera_nx.cpp).
 */

#include "nx_imgui_nx.h"
#include "../include/imgui/imgui.h"
#include <switch.h>

bool ImGui_ImplNX_Init()
{
    ImGuiIO& io = ImGui::GetIO();

    // Enable gamepad navigation
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    io.ConfigFlags  |= ImGuiConfigFlags_NavEnableGamepad;

    io.BackendPlatformName = "imgui_impl_nx";
    return true;
}

void ImGui_ImplNX_Shutdown()
{
    // Nothing to clean up.
}

void ImGui_ImplNX_NewFrame(const PadState& pad)
{
    ImGuiIO& io = ImGui::GetIO();

    u64 held = padGetButtons(&pad);

    // Helper: map a Switch button to an ImGui gamepad key
    auto mapBtn = [&](u64 nxBtn, ImGuiKey imKey) {
        io.AddKeyEvent(imKey, (held & nxBtn) != 0);
    };

    mapBtn(HidNpadButton_A,      ImGuiKey_GamepadFaceDown);   // confirm
    mapBtn(HidNpadButton_B,      ImGuiKey_GamepadFaceRight);  // back
    mapBtn(HidNpadButton_Up,     ImGuiKey_GamepadDpadUp);
    mapBtn(HidNpadButton_Down,   ImGuiKey_GamepadDpadDown);
    mapBtn(HidNpadButton_Left,   ImGuiKey_GamepadDpadLeft);
    mapBtn(HidNpadButton_Right,  ImGuiKey_GamepadDpadRight);
    mapBtn(HidNpadButton_L,      ImGuiKey_GamepadL1);
    mapBtn(HidNpadButton_R,      ImGuiKey_GamepadR1);
}
