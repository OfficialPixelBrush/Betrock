#include "mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture> textures) {
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    // Create Vertex Array Object and Bind it
    vao.Bind();

    // Generates Vertex Buffer Object and links it to vertices
    VBO vbo(vertices);
    // Same with the Element Buffer Object, just for indices
    EBO ebo(indices);

    // Link that shit to VAO
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3*sizeof(float)));
    vao.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6*sizeof(float)));
    vao.LinkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9*sizeof(float)));

    // Clean up our binds for new stuff
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera) {
    shader.Activate();
    vao.Bind();

    uint numberOfDiffuse = 0;
    uint numberOfSpecular = 0;

    for (uint i = 0; i < textures.size(); i++) {
        std::string number;
        std::string type = textures[i].type;
        if (type == "diffuse") {
            number = std::to_string(numberOfDiffuse++);
        } else if (type == "specular") {
            number = std::to_string(numberOfSpecular++); 
        }
        textures[i].textureUnit(shader, (type + number).c_str(), i);
        textures[i].Bind();
    }
    glUniform3f(glGetUniformLocation(shader.Id, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "cameraMatrix");

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}