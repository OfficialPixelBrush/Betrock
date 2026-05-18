#pragma once
/**
 * src/render/camera_nx.h
 *
 * Nintendo Switch version of the Camera class.
 * The only difference from camera.h is that ProcessInput() takes a PadState
 * reference (libnx) instead of a GLFWwindow pointer.
 *
 * On the desktop build, camera.h / camera.cpp are used unchanged.
 * On the Switch build (PLATFORM_SWITCH), CMakeLists_Switch.txt compiles
 * camera_nx.cpp instead of camera.cpp, and nx_main.cpp includes this header.
 */

#include <switch.h>          // PadState

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader.h"

class Camera
{
public:
    glm::vec3 Position    = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up          = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 Velocity    = glm::vec3(0.0f);

    float sensitivity = 2.5f;   // adjusted for stick range
    float speed       = kDefaultSpeed;

    Camera(int width, int height, glm::vec3 position, glm::vec3 orientation);

    void      UpdateMatrix(float fovDeg, float nearPlane, float farPlane);
    void      UpdateResolution(int width, int height);
    void      SetDelta(double delta);
    void      UploadMatrix(Shader& shader, const char* uniform) const;

    /** Called once per frame with the current pad state. */
    void      ProcessInput(const PadState& pad);

    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;

private:
    static constexpr float kDefaultSpeed = 0.1f;
    static constexpr float kDragFactor   = 1.1f;

    int width  = 0;
    int height = 0;

    float fovDeg    = 45.0f;
    float nearPlane = 0.1f;
    float farPlane  = 100.0f;

    glm::mat4 cameraMatrix = glm::mat4(1.0f);
    double    delta        = 0.0;
    bool      speedModified = false;
};
