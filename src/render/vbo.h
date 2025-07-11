#pragma once
#include <glm/glm.hpp>
#include "../include/glad/glad.h"
#include <vector>

#include "../compat.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    float skyLight;
    glm::vec2 textureUV;

    // Explicit constructor
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& col, const float& skyLight, const glm::vec2& uv)
        : position(pos), normal(norm), color(col), skyLight(skyLight), textureUV(uv) {}
};

class VBO {
    public:
        GLuint Id;
        VBO (std::vector<Vertex>& vertices);

        void Bind();
        void Unbind();
        void Delete();
};