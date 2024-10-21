#include "texture.h"

uint8_t missing[16] = {255,0,255,255,0,0,0,255,0,0,0,255,255,0,255,255};

Texture::Texture(const char* imagePath, const char* textureType, GLuint slot) {
    type = textureType;
    int imageWidth, imageHeight, numberOfColorChannels;
    bool success = false;
    // Flip the image to-be-loaded vertically, because STBI loads images top to bottom by default
    stbi_set_flip_vertically_on_load(true);
    // Load given image file in
    uint8_t* bytes = stbi_load(imagePath, &imageWidth, &imageHeight, &numberOfColorChannels, 0);
    // Handle missing textures// Handle missing textures
    if (!bytes) {
        std::cerr << imagePath << " not found!" << std::endl;
        // Missing texture!
        imageWidth = 32;
        imageHeight = 32;
        numberOfColorChannels = 4;
        bytes = (uint8_t*)malloc(imageWidth * imageHeight * numberOfColorChannels);

        // Loop over each 2x2 block in the 32x32 texture
        for (uint32_t y = 0; y < imageHeight; y++) {
            for (uint32_t x = 0; x < imageWidth; x++) {
                // Calculate the index in the 32x32 texture
                uint32_t index = (y * imageWidth + x) * numberOfColorChannels;

                // Calculate the corresponding pixel in the 2x2 texture
                uint32_t smallX = x % 2;
                uint32_t smallY = y % 2;
                uint32_t smallIndex = (smallY * 2 + smallX) * numberOfColorChannels;

                // Copy the pixel from the 2x2 texture
                bytes[index + 0] = missing[smallIndex + 0]; // Red
                bytes[index + 1] = missing[smallIndex + 1]; // Green
                bytes[index + 2] = missing[smallIndex + 2]; // Blue
                bytes[index + 3] = missing[smallIndex + 3]; // Alpha
            }
        }
    } else {
        success = true;
    }
    glGenTextures(1, &Id);
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, Id);

    // Set Texture Scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set how Texture Repeats
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (numberOfColorChannels == 4) {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            imageWidth,
            imageHeight,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes
        );
    } else if (numberOfColorChannels = 3) {
        glTexImage2D (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            imageWidth,
            imageHeight,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes
        );
    } else if (numberOfColorChannels = 1) {
        glTexImage2D (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            imageWidth,
            imageHeight,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bytes
        );
    } else {
        throw std::invalid_argument("Automatic Texture type recognition failed!");
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    // Delete the Image from RAM
    if (success) {
        stbi_image_free(bytes);
        std::cout << imagePath << " found!" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Yeet to shader
void Texture::textureUnit(Shader& shader, const char* uniform, GLuint unit) {
    GLuint textureUniform = glGetUniformLocation(shader.Id, uniform);
    shader.Activate();
    glUniform1i(textureUniform, unit);
}

// Use this texture
void Texture::Bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, Id);
}

// Don't use this texture
void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Delete this texture
void Texture::Delete() {
    glDeleteTextures(1, &Id);
}