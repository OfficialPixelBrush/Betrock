#include "mesh.h"

Mesh::Mesh(std::string pName, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture> textures) {
    Mesh::name = pName;
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
    /*
    std::cout << name << " is born!" << std::endl;
    std::cout << "Model with " << std::to_string(vertices.size()) << " verts, ";
    std::cout << std::to_string(indices.size()) << " indices ";
    std::cout << "and " << std::to_string(textures.size()) << " textures loaded"<< std::endl;
    */
}

Mesh::~Mesh() {
    vao.Delete();
    vertices.clear();
    indices.clear();
    textures.clear();
    //std::cout << "Deleted Mesh " << Mesh::name << std::endl;
}

void Mesh::Draw(
    Shader& shader,
    Camera& camera, 
    glm::mat4 pMatrix,
    glm::vec3 pTranslation,
    glm::quat pRotation,
    glm::vec3 pScale
) {
    //std::cout << name << " is getting drawn" << std::endl;
	// Bind shader to be able to access uniforms
    shader.Activate();
    vao.Bind();

	// Keep track of how many of each type of textures we have
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

    glm::mat4 translation = glm::mat4(1.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);

	// Use translation, rotation, and scale to change the initialized matrices
    translation = glm::translate(translation, pTranslation);
    rotation = glm::mat4_cast(pRotation);
    scale = glm::scale(scale, pScale);

	// Push the matrices to the vertex shader
    glUniformMatrix4fv(glGetUniformLocation(shader.Id, "translation"), 1, GL_FALSE, glm::value_ptr(pTranslation));
    glUniformMatrix4fv(glGetUniformLocation(shader.Id, "rotation"), 1, GL_FALSE, glm::value_ptr(pRotation));
    glUniformMatrix4fv(glGetUniformLocation(shader.Id, "scale"), 1, GL_FALSE, glm::value_ptr(pScale));
    glUniformMatrix4fv(glGetUniformLocation(shader.Id, "model"), 1, GL_FALSE, glm::value_ptr(pMatrix));

	// Draw the actual mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}