#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader.h"

class Camera {
    public:
        // Used to keep track if one is already clicked into the application
        bool firstClick = true;
        // Used to keep track of Camera Position, Orientation and Up Vector
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Camera size
        int width, height;

        // Movement Settings
        float speed = 0.03;
        float sensitivity = 150.0f;

        Camera(int pWidth, int pHeight, glm::vec3 pPosition);

        // Used to define projection Matrix
        void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
        // Used to apply movement inputs to Camera
        void Inputs(GLFWwindow* window);
};