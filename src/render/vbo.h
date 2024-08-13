#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

#include "../compat.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 textureUV;
};

class VBO {
    public:
        GLuint Id;
        VBO (std::vector<Vertex>& vertices);

        void Bind();
        void Unbind();
        void Delete();
};