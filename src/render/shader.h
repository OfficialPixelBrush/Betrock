#pragma once

#include "../include/glad/glad.h"
#include <glm/glm.hpp>
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

        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(const std::string &name, bool value) const
        {         
            glUniform1i(glGetUniformLocation(Id, name.c_str()), (int)value); 
        }
        // ------------------------------------------------------------------------
        void setInt(const std::string &name, int value) const
        { 
            glUniform1i(glGetUniformLocation(Id, name.c_str()), value); 
        }
        // ------------------------------------------------------------------------
        void setFloat(const std::string &name, float value) const
        { 
            glUniform1f(glGetUniformLocation(Id, name.c_str()), value); 
        }
        // ------------------------------------------------------------------------
        void setVec2(const std::string &name, const glm::vec2 &value) const
        { 
            glUniform2fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]); 
        }
        void setVec2(const std::string &name, float x, float y) const
        { 
            glUniform2f(glGetUniformLocation(Id, name.c_str()), x, y); 
        }
        // ------------------------------------------------------------------------
        void setVec3(const std::string &name, const glm::vec3 &value) const
        { 
            glUniform3fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]); 
        }
        void setVec3(const std::string &name, float x, float y, float z) const
        { 
            glUniform3f(glGetUniformLocation(Id, name.c_str()), x, y, z); 
        }
        // ------------------------------------------------------------------------
        void setVec4(const std::string &name, const glm::vec4 &value) const
        { 
            glUniform4fv(glGetUniformLocation(Id, name.c_str()), 1, &value[0]); 
        }
        void setVec4(const std::string &name, float x, float y, float z, float w) const
        { 
            glUniform4f(glGetUniformLocation(Id, name.c_str()), x, y, z, w); 
        }
        // ------------------------------------------------------------------------
        void setMat2(const std::string &name, const glm::mat2 &mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat3(const std::string &name, const glm::mat3 &mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat4(const std::string &name, const glm::mat4 &mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
    private:
        void compileErrors(uint shader, const char* type);
};