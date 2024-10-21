#pragma once

#include "../include/glad/glad.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include "../compat.h"

std::string getFileContents(const char* filename);

class Shader {
    public:
        GLuint Id;
        Shader(const char* vertexFile, const char* fragmentFile);

        void Activate();
        void Delete();
    private:
        void compileErrors(uint shader, const char* type);
};