#pragma once
#include <string>
#include "vao.h"
#include "ebo.h"
#include "camera.h"
#include "texture.h"

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        VAO vao;

        Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture> textures);

        void Draw(Shader& shader, Camera& camera);
};