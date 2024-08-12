#include "shader.h"

std::string getFileContents(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (file) {
        std::string contents;
        file.seekg(0, std::ios::end);
        contents.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&contents[0], contents.size());
        file.close();
        return contents;
    }
    std::cerr << "Shader file \"" << path << "\" not found" << std::endl;
    throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
    std::string vertexCode = getFileContents(vertexFile);
    std::string fragmentCode = getFileContents(fragmentFile);
    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // Create Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    // Create Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    // Create Shader Program and Link
    Id = glCreateProgram();
    glAttachShader(Id, vertexShader);
    glAttachShader(Id, fragmentShader);
    glLinkProgram(Id);
    compileErrors(Id, "PROGRAM");

    // Delete Vertex and Fragment Shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate() {
    glUseProgram(Id);
}

void Shader::Delete() {
    glDeleteProgram(Id);
}

void Shader::compileErrors(uint shader, const char* type) {
    GLint hasCompiled;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "SHADER_COMP_ERROR for: " << type << std::endl;
        }
    } else {
        
    }
}