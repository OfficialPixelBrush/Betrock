#pragma once
#include <glad/glad.h>
#include <stb/stb_image.h>
#include "shader.h"

class Texture {
    public:
        GLuint Id;
        GLenum type;
        Texture(const char* imagePath, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType);

        void textureUnit(Shader& shader, const char* uniform, GLuint unit);
        void Bind();
        void Unbind();
        void Delete();
};