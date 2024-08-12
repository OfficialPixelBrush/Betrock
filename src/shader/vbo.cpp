#include "vbo.h"

VBO::VBO (GLfloat* vertices, GLsizeiptr size) {
    glGenBuffers(1, &Id);
    // Set the VBO as our current target object
    glBindBuffer(GL_ARRAY_BUFFER, Id);

    // Load the vertices into the VBO
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, Id);
}

void VBO::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() {
    glDeleteBuffers(1, &Id);
}