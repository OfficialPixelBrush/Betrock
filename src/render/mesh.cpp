#include "mesh.h"

Mesh::Mesh(std::string pName, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture> textures)
    : vbo(vertices), ebo(indices) {
    if (vertices.empty() && indices.empty()) {
        std::cout << "Missing Vertices and Indices!" << std::endl;
    } else if (vertices.empty()) {
        std::cout << "Missing Vertices" << std::endl;
    } else if (indices.empty()) {
        std::cout << "Missing Indices!" << std::endl;
    }
    Mesh::name = pName;
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    // Create Vertex Array Object and Bind it
    vao.Bind();
    vbo.Bind();

    // Link that shit to VAO
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    vao.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));
    vao.LinkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, textureUV));
    vao.LinkAttrib(vbo, 4, 1, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, skyLight));
    vao.LinkAttrib(vbo, 5, 1, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, blockLight));

    ebo.Bind();

    // Clean up our binds for new stuff
    vao.Unbind();
}

Mesh::~Mesh() {
    vao.Unbind();
    vao.Delete();

    vertices.clear();
    indices.clear();
    textures.clear();
    std::cout << "Deleted " << name << " Mesh " << this << std::endl;
}

void Mesh::Draw(
    Shader& shader,
    Camera& camera, 
    glm::mat4 pMatrix,
    glm::vec3 pTranslation,
    glm::quat pRotation,
    glm::vec3 pScale
) {
    //std::cout << this << " (" << Mesh::name << ") is getting drawn" << std::endl;
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
    glm::mat4 model = translation * rotation * scale;
    glUniformMatrix4fv(glGetUniformLocation(shader.Id, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// Draw the actual mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL error after draw: " << err << std::endl;
    }
    vao.Unbind();
}