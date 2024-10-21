#pragma once
#include "../include/glad/glad.h"
#include <vector>
#include "../compat.h"

class EBO {
    public:
        GLuint Id;
        EBO (std::vector<GLuint>& indices);

        void Bind();
        void Unbind();
        void Delete();
};