#include "camera.h"

Camera::Camera(int pWidth, int pHeight, glm::vec3 pPosition, glm::vec3 pOrientation) {
    Camera::width = pWidth;
    Camera::height = pHeight;
    Position = pPosition;
    Orientation = pOrientation;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(Position, Position + Orientation, Up);
    projection = glm::perspective(glm::radians(FOVdeg), (float)((float)width/(float)height), nearPlane, farPlane);
    cameraMatrix = projection * view;
}

void Camera::updateResolution(int pWidth, int pHeight) {
    Camera::width = pWidth;
    Camera::height = pHeight;
}

void Camera::Matrix(Shader& shader, const char* uniform) {
    glUniformMatrix4fv(glGetUniformLocation(shader.Id, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::setDelta(double delta) {
    Camera::delta = delta;
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Orientation, Up);
}

void Camera::Inputs(GLFWwindow* window) {
    float tSpeed = speed * delta;
    // WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Velocity += tSpeed * Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Velocity += tSpeed * -glm::normalize(glm::cross(Orientation, Up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Velocity += tSpeed * -Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Velocity += tSpeed * glm::normalize(glm::cross(Orientation, Up));
    }
    // Going up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Velocity += tSpeed * Up;
    }
    // Going down
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        Velocity += tSpeed * -Up;
    }
    
    // 0 to Reset Speed
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        speed = 0.1;
    }
    // q to increase Speed
    if ((glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) && (speedModified == false)) {
        speed *= 2;
        speedModified = true;
    }
    
    // e to lower speed
    if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (speedModified == false)) {
        speed *= 0.5;
        speedModified = true;
    }

    if ((glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) && (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) && (speedModified == true)) {
        speedModified = false;
    }


    // Click into Window
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        if (firstClick) {
            firstClick = false;
            glfwSetCursorPos(window, (width/2), (height/2));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }

    if (!firstClick) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotationX = sensitivity * (float)(mouseY - ((float)height / 2.0)) / (float)height;
        float rotationY = sensitivity * (float)(mouseX - ((float)width / 2.0)) / (float)width;

        // Calculate new orientation based on rotation around right axis (to avoid flipping upside down)
        glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotationX), right);

        // Prevent flipping by clamping the angle between newOrientation and the Up vector
        float angleFromUp = glm::angle(newOrientation, Up);

        // Allow rotation only if the angle from Up is between 10 degrees and 170 degrees
        if (angleFromUp > glm::radians(10.0f) && angleFromUp < glm::radians(170.0f)) {
            Orientation = newOrientation;
        }

        // Rotate around the Up vector (horizontal rotation)
        Orientation = glm::rotate(Orientation, glm::radians(-rotationY), Up);

        // Reset the mouse cursor position to the center of the screen
        glfwSetCursorPos(window, (width / 2), (height / 2));
    }
    Position += Velocity;

    // Apply Drag
    if (Velocity.x != 0.0) {
        Velocity.x /= 1.1;
    }
    if (Velocity.y != 0.0) {
        Velocity.y /= 1.1;
    }
    if (Velocity.z != 0.0) {
        Velocity.z /= 1.1;
    }
}