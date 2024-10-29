#pragma once
#include "../world/chunk.h"
#include "../render/mesh.h"
#include "../model/model.h"

class DummyMesh {
    public:
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        Chunk* chunk;
        DummyMesh(Chunk* chunk, std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
            this->chunk = chunk;
            this->name = name;
            this->vertices = vertices;
            this->indices = indices;
        }
};