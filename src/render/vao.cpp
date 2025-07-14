#include "vao.h"

VAO::VAO() {
    glGenVertexArrays(1, &Id);
    if (!Id) {
        int x;
        std::cout << "VAO DID NOT RECEIVE ID!!!" << std::endl;
    }
}

void VAO::LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
    vbo.Bind();
    // Configure the Vertex Attribute so OpenGL knows how to read the VBO
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    // Enable the Vertex Attribute so that OpenGL knows to use it
    glEnableVertexAttribArray(layout);
    vbo.Unbind();
}

void VAO::Bind() {
    std::cout << "VAO ID before bind: " << Id << std::endl;
    // Set VAO as current target
    glBindVertexArray(Id);
}

void VAO::Unbind() {
    std::cout << "VAO #" << Id << " is being unbound!" << std::endl;
    // Set VAO as current target
    glBindVertexArray(0);
}

void VAO::Delete() {
    std::cout << "VAO #" << Id << " is being deleted!" << std::endl;
    // Set VAO as current target
    glDeleteVertexArrays(1, &Id);
}