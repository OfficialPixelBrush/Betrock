/**
 * src/render/camera_nx.cpp
 *
 * Nintendo Switch camera – uses libnx padStateUpdate / hidAnalogStickVal
 * for movement and look instead of GLFW keyboard + mouse.
 *
 * Control mapping:
 *   Left  stick  →  move (forward/back/strafe)
 *   Right stick  →  look (yaw/pitch)
 *   ZR           →  fly up
 *   ZL           →  fly down
 *   R            →  double speed (hold)
 *   L            →  half   speed (hold)
 *   Minus        →  reset speed to default
 */

#include "camera_nx.h"

// ── libnx stick helpers ───────────────────────────────────────────────────────
// hidAnalogStickVal returns values in [-32767, 32767]
static constexpr float kStickMax   = 32767.0f;
static constexpr float kDeadzone   = 0.10f;  // 10 % dead-zone

static float stickAxis(s32 raw)
{
    float v = raw / kStickMax;
    if (v >  kDeadzone) return (v - kDeadzone) / (1.0f - kDeadzone);
    if (v < -kDeadzone) return (v + kDeadzone) / (1.0f - kDeadzone);
    return 0.0f;
}

// ── Constructor / helpers (identical to desktop) ──────────────────────────────
Camera::Camera(int w, int h, glm::vec3 position, glm::vec3 orientation)
    : width(w), height(h), Position(position), Orientation(orientation)
{}

void Camera::UpdateMatrix(float fovDeg_, float near_, float far_)
{
    fovDeg    = fovDeg_;
    nearPlane = near_;
    farPlane  = far_;

    glm::mat4 view       = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4 projection = GetProjectionMatrix();
    cameraMatrix         = projection * view;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    return glm::perspective(glm::radians(fovDeg), aspect, nearPlane, farPlane);
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Orientation, Up);
}

void Camera::UpdateResolution(int w, int h) { width = w; height = h; }
void Camera::SetDelta(double d)             { delta = d; }

void Camera::UploadMatrix(Shader& shader, const char* uniform) const
{
    glUniformMatrix4fv(
        glGetUniformLocation(shader.Id, uniform),
        1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

// ── Switch input processing ───────────────────────────────────────────────────
void Camera::ProcessInput(const PadState& pad)
{
    u64 held  = padGetButtons(&pad);
    u64 down  = padGetButtonsDown(&pad);

    // ── Speed control ─────────────────────────────────────────────────────────
    if (down & HidNpadButton_Minus) speed = kDefaultSpeed;

    const bool rHeld = held & HidNpadButton_R;
    const bool lHeld = held & HidNpadButton_L;

    if (!speedModified) {
        if (rHeld) { speed *= 2.0f;  speedModified = true; }
        if (lHeld) { speed *= 0.5f;  speedModified = true; }
    }
    if (!rHeld && !lHeld) speedModified = false;

    // ── Movement (left stick) ─────────────────────────────────────────────────
    HidAnalogStickState leftStick = padGetStickPos(&pad, 0);
    float lx = stickAxis(leftStick.x);
    float ly = stickAxis(leftStick.y);   // positive = forward on Switch

    const float tSpeed = static_cast<float>(speed * delta);
    const glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));

    Velocity +=  ly * tSpeed * Orientation;   // forward / back
    Velocity +=  lx * tSpeed * right;         // strafe

    // Vertical movement
    if (held & HidNpadButton_ZR) Velocity +=  tSpeed * Up;
    if (held & HidNpadButton_ZL) Velocity += -tSpeed * Up;

    // ── Look (right stick) ────────────────────────────────────────────────────
    HidAnalogStickState rightStick = padGetStickPos(&pad, 1);
    float rx = stickAxis(rightStick.x);
    float ry = stickAxis(rightStick.y);

    if (rx != 0.0f || ry != 0.0f) {
        // Yaw: rotate around world Up
        float yawDelta = -rx * sensitivity * static_cast<float>(delta) * 0.05f;
        Orientation    = glm::rotate(Orientation, glm::radians(yawDelta), Up);

        // Pitch: rotate around local right, clamped to avoid gimbal flipping
        float pitchDelta        = ry * sensitivity * static_cast<float>(delta) * 0.05f;
        const glm::vec3 r       = glm::normalize(glm::cross(Orientation, Up));
        const glm::vec3 newOri  = glm::rotate(Orientation, glm::radians(pitchDelta), r);
        const float angle       = glm::angle(newOri, Up);
        if (angle > glm::radians(10.0f) && angle < glm::radians(170.0f))
            Orientation = newOri;
    }

    // ── Apply velocity ────────────────────────────────────────────────────────
    Position += Velocity;
    Velocity /= kDragFactor;
}
