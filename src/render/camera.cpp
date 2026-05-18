#include "camera_nx.h"

Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 orientation)
    : width(width), height(height), Position(position), Orientation(orientation)
{
}

void Camera::UpdateMatrix(float fovDeg, float newNearPlane, float newFarPlane)
{
    FOVdeg    = fovDeg;
    nearPlane = newNearPlane;
    farPlane  = newFarPlane;

    glm::mat4 view       = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4 projection = GetProjectionMatrix();
    cameraMatrix         = projection * view;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    return glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Orientation, Up);
}

void Camera::UpdateResolution(int newWidth, int newHeight)
{
    width  = newWidth;
    height = newHeight;
}

void Camera::SetDelta(double newDelta)
{
    delta = newDelta;
}

void Camera::UploadMatrix(Shader& shader, const char* uniform) const
{
    glUniformMatrix4fv(
        glGetUniformLocation(shader.Id, uniform),
        1, GL_FALSE,
        glm::value_ptr(cameraMatrix)
    );
}

void Camera::ProcessInput(GLFWwindow* window)
{
    ProcessMovement(window);
    ProcessSpeedControl(window);
    ProcessCursorControl(window);

    Position += Velocity;
    Velocity /= kDragFactor;
}

// --- Private helpers ---

void Camera::ProcessMovement(GLFWwindow* window)
{
    const float tSpeed = static_cast<float>(speed * delta);
    const glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Velocity +=  tSpeed * Orientation;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Velocity += -tSpeed * Orientation;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Velocity += -tSpeed * right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Velocity +=  tSpeed * right;

    if (glfwGetKey(window, GLFW_KEY_SPACE)      == GLFW_PRESS) Velocity +=  tSpeed * Up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) Velocity += -tSpeed * Up;
}

void Camera::ProcessSpeedControl(GLFWwindow* window)
{
    const bool qPressed = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
    const bool ePressed = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        speed = kDefaultSpeed;

    if (!speedModified) {
        if (qPressed) { speed *= 2.0f;  speedModified = true; }
        if (ePressed) { speed *= 0.5f;  speedModified = true; }
    }

    if (!qPressed && !ePressed)
        speedModified = false;
}

void Camera::ProcessCursorControl(GLFWwindow* window)
{
    const bool mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    const bool escDown   = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (mouseDown) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorCaptured = true;
    }
    if (escDown) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorCaptured = false;
    }

    if (!cursorCaptured) return;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    const float deltaX = static_cast<float>(mouseX - lastMouseX);
    const float deltaY = static_cast<float>(mouseY - lastMouseY);
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    const float pitchDelta = sensitivity * deltaY / static_cast<float>(height);
    const float yawDelta   = sensitivity * deltaX / static_cast<float>(width);

    // Pitch (rotate around right vector), clamped away from the poles
    const glm::vec3 right          = glm::normalize(glm::cross(Orientation, Up));
    const glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-pitchDelta), right);
    const float     angleFromUp    = glm::angle(newOrientation, Up);

    if (angleFromUp > glm::radians(10.0f) && angleFromUp < glm::radians(170.0f))
        Orientation = newOrientation;

    // Yaw (rotate around world Up)
    Orientation = glm::rotate(Orientation, glm::radians(-yawDelta), Up);
}