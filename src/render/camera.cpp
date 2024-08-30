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

void Camera::Matrix(Shader& shader, const char* uniform) {
    glUniformMatrix4fv(glGetUniformLocation(shader.Id, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}


void Camera::Inputs(GLFWwindow* window) {
    // WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Position += speed * Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Position += speed * -glm::normalize(glm::cross(Orientation, Up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Position += speed * -Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Position += speed * glm::normalize(glm::cross(Orientation, Up));
    }
    // Going up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Position += speed * Up;
    }
    // Going down
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        Position += speed * -Up;
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
        glfwGetCursorPos(window,&mouseX, &mouseY);

        float rotationX = sensitivity * (float)(mouseY - ((float)height / 2.0)) / (float)height;
        float rotationY = sensitivity * (float)(mouseX - ((float)width / 2.0)) / (float)width;

        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotationX), glm::normalize(glm::cross(Orientation, Up)));

        if (!(glm::angle(newOrientation, Up) <= glm::radians(5.0f)) ||
        (glm::angle(newOrientation, -Up) <= glm::radians(5.0f)))  {
            Orientation = newOrientation;
        }

        Orientation = glm::rotate(Orientation, glm::radians(-rotationY), Up);

        glfwSetCursorPos(window, (width/2), (height/2));
    }
}