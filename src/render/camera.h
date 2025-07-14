#pragma once
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "../compat.h"

#include "shader.h"

class Camera {
    public:
        // Used to keep track if one is already clicked into the application
        bool firstClick = true;
        // Used to keep track of Camera Position, Orientation and Up Vector
        glm::vec3 Position;
        glm::vec3 Velocity = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec3 Drag = glm::vec3(0.09f,0.02f,0.09f);
        glm::vec3 Orientation;
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 cameraMatrix = glm::mat4(1.0f);

        // Camera size
        int width, height;

        float FOVdeg;
        float nearPlane;
        float farPlane;

        // Movement Settings
        float speed = 0.01;
        bool speedModified = false;
        double delta = 1.0f;
        float sensitivity = 150.0f;

        Camera(int pWidth, int pHeight, glm::vec3 pPosition, glm::vec3 pOrientation = glm::vec3(0.0f, 0.0f, -1.0f));

        // Used to define projection Matrix
        void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
        glm::mat4 GetProjectionMatrix();
        void updateResolution(int pWidth, int pHeight);
        void Matrix(Shader& shader, const char* uniform);

        // Used to apply movement inputs to Camera
        void Inputs(GLFWwindow* window);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        void setDelta(double delta);
        glm::mat4 GetViewMatrix();
};