#include "vao.h"

VAO::VAO() {
    glGenVertexArrays(1, &Id);
}

void VAO::LinkAttrib(VBO vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
    vbo.Bind();
    // Configure the Vertex Attribute so OpenGL knows how to read the VBO
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    // Enable the Vertex Attribute so that OpenGL knows to use it
    glEnableVertexAttribArray(layout);
    vbo.Unbind();
}

void VAO::Bind() {
    // Set VAO as current target
    glBindVertexArray(Id);
}

void VAO::Unbind() {
    // Set VAO as current target
    glBindVertexArray(0);
}

void VAO::Delete() {
    // Set VAO as current target
    glDeleteVertexArrays(1, &Id);
}