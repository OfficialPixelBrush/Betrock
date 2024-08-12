#pragma once
#include <glad/glad.h>
#include "vbo.h"

class VAO {
    public:
        GLuint Id;
        VAO();

        void LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
        void Bind();
        void Unbind();
        void Delete();
};