#include "texture.h"

uint8_t missing[2*2*4] = {255,0,255,255,0,0,0,255,0,0,0,255,255,0,255,255};

Texture::Texture(const char* imagePath, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType) {
    type = textureType;
    int imageWidth, imageHeight, numberOfColorChannels;
    bool success = false;
    // Flip the image to-be-loaded vertically, because STBI loads images top to bottom by default
    stbi_set_flip_vertically_on_load(true);
    // Load given image file in
    uint8_t* bytes = stbi_load(imagePath, &imageWidth, &imageHeight, &numberOfColorChannels, 0);
    // Handle missing textures
    if (!bytes) {
        std::cerr << imagePath << " not found!" << std::endl;
        // Missing texture!
        imageWidth = 2;
        imageHeight = 2;
        numberOfColorChannels = 4;
        bytes = missing;
    } else {
        success = true;
    }
    glGenTextures(1, &Id);
    glActiveTexture(slot);
    glBindTexture(textureType, Id);

    // Set Texture Scaling
    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set how Texture Repeats
    glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(textureType, 0, GL_RGBA, imageWidth, imageHeight, 0, format, pixelType, bytes);
    glGenerateMipmap(textureType);

    // Delete the Image from RAM
    if (success) {
        stbi_image_free(bytes);
    }
    glBindTexture(textureType, 0);
}

// Yeet to shader
void Texture::textureUnit(Shader& shader, const char* uniform, GLuint unit) {
    GLuint textureUniform = glGetUniformLocation(shader.Id, uniform);
    shader.Activate();
    glUniform1i(textureUniform, unit);
}

// Use this texture
void Texture::Bind() {
    glBindTexture(type, Id);
}

// Don't use this texture
void Texture::Unbind() {
    glBindTexture(type, 0);
}

// Delete this texture
void Texture::Delete() {
    glDeleteTextures(1, &Id);
}