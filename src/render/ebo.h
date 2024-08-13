#pragma once
#include <glad/glad.h>
#include "../compat.h"

class EBO {
    public:
        GLuint Id;
        EBO (GLuint* indices, GLsizeiptr size);

        void Bind();
        void Unbind();
        void Delete();
};