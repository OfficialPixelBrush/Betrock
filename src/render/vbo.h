#pragma once
#include <glm/glm.hpp>
#include "../include/glad/glad.h"
#include <vector>

#include "../compat.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 textureUV;
    float skyLight;
    float blockLight;

    // Explicit constructor
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& col, const glm::vec2& uv, const float& skyLight, const float& blockLight)
        : position(pos), normal(norm), color(col), textureUV(uv), skyLight(skyLight), blockLight(blockLight) {}
};

class VBO {
    public:
        GLuint Id;
        VBO (std::vector<Vertex>& vertices);

        void Bind();
        void Unbind();
        void Delete();
};