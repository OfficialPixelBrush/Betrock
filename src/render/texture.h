#pragma once
#include <glad/glad.h>
#include <stb/stb_image.h>
#include "shader.h"
#include "../compat.h"

class Texture {
    public:
        GLuint Id;
        const char* type;
        GLuint unit;
        Texture(const char* imagePath, const char* textureType, GLuint slot);

        void textureUnit(Shader& shader, const char* uniform, GLuint unit);
        void Bind();
        void Unbind();
        void Delete();
};