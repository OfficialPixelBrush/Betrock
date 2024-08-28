#pragma once
#include <string>
#include "vao.h"
#include "ebo.h"
#include "camera.h"
#include "texture.h"

class Mesh {
    public:
        std::string name;

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        VAO vao;

        Mesh(std::string pName, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture> textures);

        void Draw(
            Shader& shader,
            Camera& camera,
            glm::mat4 pMatrix = glm::mat4(1.0f),
            glm::vec3 pTranslation = glm::vec3(0.0f,0.0f,0.0f),
            glm::quat pRotation = glm::quat(1.0f,0.0f,0.0f,0.0f),
            glm::vec3 pScale = glm::vec3(1.0f, 1.0f, 1.0f)
        );
};