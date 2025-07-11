#include "model.h"

Model::Model(const char* file) {
	// Get the binary data
    Model::file = file;
    getMeshData();
}

void Model::Draw(Shader& shader, Camera& camera) {
	// Go over all meshes and draw each one
    for (uint i = 0; i < meshes.size(); i++) {
        meshes[i].Mesh::Draw(shader, camera);
    }
}

void Model::getMeshData() {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textureUVs;
    std::vector<glm::vec3> normals;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    std::string objectName;
    bool firstObject = true;
    GLuint indexCount = 0;

    std::ifstream source(file, std::ios_base::in);
    if (!source) {
        throw std::runtime_error("Could not open file: " + std::string(file));
    }

    auto parseFaceVertex = [&](const std::string& vertex) -> std::tuple<GLuint, GLuint, GLuint> {
        // Parse a face vertex string "pos/uv/norm", where uv or norm can be missing.
        GLuint pos = 0, texUV = 0, norm = 0;
        size_t firstSlash = vertex.find('/');
        size_t secondSlash = vertex.find('/', firstSlash + 1);

        try {
            if (firstSlash == std::string::npos) {
                // Only position index
                pos = std::stoul(vertex);
            } else if (secondSlash == std::string::npos) {
                // pos/uv
                pos = std::stoul(vertex.substr(0, firstSlash));
                std::string uvStr = vertex.substr(firstSlash + 1);
                if (!uvStr.empty()) texUV = std::stoul(uvStr);
            } else {
                // pos/uv/norm or pos//norm
                pos = std::stoul(vertex.substr(0, firstSlash));
                std::string uvStr = vertex.substr(firstSlash + 1, secondSlash - firstSlash - 1);
                std::string normStr = vertex.substr(secondSlash + 1);
                if (!uvStr.empty()) texUV = std::stoul(uvStr);
                if (!normStr.empty()) norm = std::stoul(normStr);
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse face vertex: " << vertex << " (" << e.what() << ")\n";
        }
        return {pos, texUV, norm};
    };

    for (std::string line; std::getline(source, line); ) {
        if (line.empty()) continue;

        std::istringstream in(line);
        std::string type;
        in >> type;
        if (type == "v") {
            // Vertex position
            float x, y, z;
            in >> x >> y >> z;
            positions.emplace_back(x, y, z);

        } else if (type == "vt") {
            // Texture UV
            float u, v;
            in >> u >> v;
            textureUVs.emplace_back(u, v);

        } else if (type == "vn") {
            // Normal vector
            float x, y, z;
            in >> x >> y >> z;
            normals.emplace_back(x, y, z);

        } else if (type == "f") {
            // Face (triangle or polygon)
            std::string vertexStr;
            while (in >> vertexStr) {
                auto [pos, texUV, norm] = parseFaceVertex(vertexStr);

                // Validate indices
                if (pos == 0 || pos > positions.size()) {
                    std::cerr << "Invalid position index in face: " << pos << std::endl;
                    continue;
                }
                glm::vec3 position = positions[pos - 1];

                glm::vec2 uv(0.0f);
                if (texUV != 0) {
                    if (texUV > textureUVs.size()) {
                        std::cerr << "Invalid texture UV index in face: " << texUV << std::endl;
                        continue;
                    }
                    uv = textureUVs[texUV - 1];
                }

                glm::vec3 normal(0.0f, 0.0f, 0.0f);
                if (norm != 0) {
                    if (norm > normals.size()) {
                        std::cerr << "Invalid normal index in face: " << norm << std::endl;
                        continue;
                    }
                    normal = normals[norm - 1];
                }

                vertices.emplace_back(
                    position,
                    normal,
                    glm::vec3(1.0f), // color or whatever your Vertex expects
                    uv,
                    1.0f, 1.0f      // any other parameters
                );
                indices.push_back(indexCount++);
            }

        } else if (type == "o") {
            // New object delimiter
            if (!firstObject) {
                meshes.push_back(Mesh(objectName, vertices, indices, getTextures()));
                vertices.clear();
                indices.clear();
                indexCount = 0;
            } else {
                firstObject = false;
            }
            in >> objectName;

        } else if (type == "#" || type.empty()) {
            // Comment or empty line, ignore
            continue;
        } else {
            // Unknown line type, ignore or handle
            // std::cout << "Unknown OBJ line type: " << type << std::endl;
        }
    }

    // Push last mesh if any
    if (!firstObject) {
        meshes.push_back(Mesh(objectName, vertices, indices, getTextures()));
    }

    std::cout << "Saved " << meshes.size() << " meshes" << std::endl;
}

std::vector<Texture> Model::getTextures() {
    std::vector<Texture> textures;
    textures.push_back(Texture("./textures/terrain.png", "diffuse", 0));
    return textures;
}