#pragma once
#include <glad/glad.h>

class VBO {
    public:
        GLuint Id;
        VBO (GLfloat* vertices, GLsizeiptr size);

        void Bind();
        void Unbind();
        void Delete();
};