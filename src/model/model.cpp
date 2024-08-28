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
    std::vector<glm::vec3> position;
    std::vector<glm::vec2> textureUVs;
    std::vector<glm::vec3> normals;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::string objectName;

    std::ifstream source;
    source.open(file, std::ios_base::in);
    if (!source)  {
        throw std::runtime_error("Could not open file: " + std::string(file));
    }
    for(std::string line; std::getline(source, line); ) {
        std::istringstream in(line);      //make a stream for the line itself

        std::string type;
        in >> type;                  //and read the first whitespace-separated token

        if(type == "v") { // Vertex Info
            float x,y,z;
            in >> x >> y >> z;
            //std::cout << x << "," << y << "," << z << std::endl;
            position.push_back(glm::vec3(x,y,z));
        } else if (type == "vt") { // UV Info
            float u,v;
            in >> u >> v;
            textureUVs.push_back(glm::vec2(u,v));
        } else if (type == "vn") { // Normal Info
            float x,y,z;
            in >> x >> y >> z;
            normals.push_back(glm::vec3(x,y,z));
        } else if (type == "f") {
            std::string vertex;
            while (in >> vertex) {
                std::replace(vertex.begin(), vertex.end(), '/', ' ');  // Replace '/' with spaces
                std::stringstream vertexStream(vertex);
                
                GLuint vIndex, vtIndex, vnIndex;
                vertexStream >> vIndex >> vtIndex >> vnIndex;
                indices.push_back(vIndex);

                // Get indicies
                vertices.push_back(
                    Vertex(
                        position[vIndex],
                        normals[vnIndex],
                        glm::vec3(1.0f,1.0f,1.0f),
                        textureUVs[vtIndex]
                    )
                );
            }
        } else if (type == "#") { // Comment
            //std::cout << line << std::endl;
        } else if (type == "o") { // Object name and delimiter
            in >> objectName;
            std::cout << "Found " << objectName << std::endl;
        }
    }
    meshes.push_back(Mesh(objectName, vertices,indices, getTextures()));
}

std::vector<Texture> Model::getTextures() {
    std::vector<Texture> textures;

    /*
    std::string fileString = std::string(file);
    std::string fileDirectory = fileString.substr(0, fileString.find_last_of('/') + 1);

    for (uint i = 0; i < Json["images"].size(); i++) {
        std::string texturePath = Json["images"][i]["uri"];

        bool skip = false;
        for (uint j = 0; j < loadedTextureName.size(); j++) {
            if (loadedTextureName[j] == texturePath) {
                textures.push_back(loadedTexture[j]);
                skip = true;
                break;
            }
        }
        
        if (!skip) {
            if (texturePath.find("baseColor") != std::string::npos) {
                Texture diffuse = Texture((fileDirectory + texturePath).c_str(), "diffuse", loadedTexture.size());
                textures.push_back(diffuse);
                loadedTexture.push_back(diffuse);
                loadedTextureName.push_back(texturePath);
            } else if (texturePath.find("metallicRoughness") != std::string::npos) {
                Texture specular = Texture((fileDirectory + texturePath).c_str(), "specular", loadedTexture.size());
                textures.push_back(specular);
                loadedTexture.push_back(specular);
                loadedTextureName.push_back(texturePath);
            }
        }
    }*/
    textures.push_back(Texture("textures/terrain.png", "diffuse", 0));
    return textures;
}