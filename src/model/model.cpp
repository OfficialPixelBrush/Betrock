#include "model.h"

std::string get_file_contents(const char* filePath) {
    std::ifstream file(filePath); // Open the file
    if (!file) {
        throw std::runtime_error("Could not open file: " + std::string(filePath));
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // Read the file into the stringstream
    return buffer.str();    // Return the string from the stringstream
}

Model::Model(const char* file) {
	// Make a JSON object
    std::string text = get_file_contents(file);
    Json = json::parse(text);

	// Get the binary data
    Model::file = file;
    data = getData();

	// Traverse all nodes
    traverseNode(0);
}

void Model::Draw(Shader& shader, Camera& camera) {
	// Go over all meshes and draw each one
    for (uint i = 0; i < meshes.size(); i++) {
        meshes[i].Mesh::Draw(shader, camera, matricesMeshes[i]);
    }
}

void Model::loadMesh(uint indexMesh) {
	// Get all accessor indices
    uint positionAccessorIndex = Json["meshes"][indexMesh]["primitives"][0]["attributes"]["POSITION"];
    uint normalAccessorIndex = Json["meshes"][indexMesh]["primitives"][0]["attributes"]["NORMAL"];
    uint textureUVAccessorIndex = Json["meshes"][indexMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
    uint indicesAccessorIndex = Json["meshes"][indexMesh]["primitives"][0]["indices"];

	// Use accessor indices to get all vertices components
    std::vector<float> positionVectors = getFloats(Json["accessors"][positionAccessorIndex]);
    std::vector<glm::vec3> positions = groupFloatsVec3(positionVectors);
    std::vector<float> normalVectors = getFloats(Json["accessors"][normalAccessorIndex]);
    std::vector<glm::vec3> normals = groupFloatsVec3(normalVectors);
    std::vector<float> textureUVVectors = getFloats(Json["accessors"][textureUVAccessorIndex]);
    std::vector<glm::vec2> textureUVs = groupFloatsVec2(textureUVVectors);

	// Combine all the vertex components and also get the indices and textures
    std::vector<Vertex> vertices = assembleVertices(positions, normals, textureUVs);
    std::vector<GLuint> indices = getIndices(Json["accessors"][indicesAccessorIndex]);
    std::vector<Texture> textures = getTextures();

    meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::traverseNode(uint nextNode, glm::mat4 matrix) {
    json node = Json["nodes"][nextNode];
    // Get Transformation
    glm::vec3 translation = glm::vec3(0.0f,0.0f,0.0f);
    if (node.find("translation") != node.end()) {
        float translationValues[3];
        for (uint i = 0; i < node["translation"].size(); i++) {
            translationValues[i] = (node["translation"][i]);
        }
        translation = glm::make_vec3(translationValues);
    }
    // Get rotation
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (node.find("rotation") != node.end()) {
        float rotationValues[4] = {
            node["rotation"][3],
            node["rotation"][0],
            node["rotation"][1],
            node["rotation"][2]
        };
        rotation = glm::make_quat(rotationValues);
    }
    // Get scale
    glm::vec3 scale = glm::vec3(1.0f,1.0f,1.0f);
    if (node.find("scale") != node.end()) {
        float scaleValues[3];
        for (uint i = 0; i < node["scale"].size(); i++) {
            scaleValues[i] = (node["scale"][i]);
        }
        scale = glm::make_vec3(scaleValues);
    }
    // Get Matrix
    glm::mat4 matrixNode = glm::mat4(1.0f);
    if (node.find("matrix") != node.end()) {
        float matrixValues[16];
        for (uint i = 0; i < node["matrix"].size(); i++) {
            matrixValues[i] = (node["matrix"][i]);
        }
        matrixNode = glm::make_mat4(matrixValues);
    }

	// Initialize matrices
    glm::mat4 translationMatrix = glm::mat4(1.0f);
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    glm::mat4 scaleMatrix = glm::mat4(1.0f);

	// Use translation, rotation, and scale to change the initialized matrices
    translationMatrix = glm::translate(translationMatrix, translation);
    rotationMatrix = glm::mat4_cast(rotation);
    scaleMatrix = glm::scale(scaleMatrix, scale);

	// Multiply all matrices together
    glm::mat4 matrixNextNode = matrix * matrixNode * translationMatrix * rotationMatrix * scaleMatrix;

    if (node.find("mesh") != node.end()) {
        translationsMeshes.push_back(translation);
        rotationsMeshes.push_back(rotation);
        scalesMeshes.push_back(scale);
        matricesMeshes.push_back(matrixNextNode);
        loadMesh(node["mesh"]);
    }
    
    // Go over children of mesh and pass transform 
    if (node.find("children") != node.end()) {
        for (uint i = 0; i < node["children"].size(); i++) {
            traverseNode(node["children"][i], matrixNextNode);
        }
    }
}

std::vector<unsigned char> Model::getData() {
    std::string bytesText;
    std::string uri = Json["buffers"][0]["uri"];

    std::string fileString = std::string(file);
    std::string fileDirectory = fileString.substr(0, fileString.find_last_of('/') + 1);
    bytesText = get_file_contents((fileDirectory + uri).c_str());
    
    std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
    return data;
}

std::vector<float> Model::getFloats(json accessor) {
    std::vector<float> floatVec;
    
    uint bufferViewIndex = accessor.value("bufferView", 1);
    uint count = accessor["count"];
    uint accessorByteOffset = accessor.value("byteOffset", 0);
    std::string type =  accessor["type"];

    json bufferView = Json["bufferViews"][bufferViewIndex];
    uint byteOffset = bufferView["byteOffset"];

    uint numberPerVertex;
    if (type == "SCALAR") numberPerVertex = 1;
    else if (type == "VEC2") numberPerVertex = 2;
    else if (type == "VEC3") numberPerVertex = 3;
    else if (type == "VEC4") numberPerVertex = 4;
    else throw std::invalid_argument("Type is invalid! (not SCALAR, VEC2, VEC3 or VEC4)");

    uint beginningOfData = byteOffset + accessorByteOffset;
    uint lengthOfData = count * 4 * numberPerVertex;
    for (uint i = beginningOfData; i < beginningOfData + lengthOfData; i) {
        unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }
    return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor) {
    std::vector<GLuint> indices;
    
    uint bufferViewIndex = accessor.value("bufferView", 1);
    uint count = accessor["count"];
    uint accessorByteOffset = accessor.value("byteOffset", 0);
    uint componentType = accessor["componentType"];

    json bufferView = Json["bufferViews"][bufferViewIndex];
    uint byteOffset = bufferView["byteOffset"];

    uint beginningOfData = byteOffset + accessorByteOffset;
    if (componentType == 5125) {
        for (uint i = beginningOfData; i < byteOffset + accessorByteOffset + count * 4; i) {
            unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
            uint value;
            std::memcpy(&value, bytes, sizeof(uint));
            indices.push_back((GLuint)value);
        }
    } else if (componentType == 5123) {
        for (uint i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2; i) {
            unsigned char bytes[] = { data[i++], data[i++] };
            ushort value;
            std::memcpy(&value, bytes, sizeof(ushort));
            indices.push_back((GLuint)value);
        }
    } else if (componentType == 5122) {
        for (uint i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2; i) {
            unsigned char bytes[] = { data[i++], data[i++] };
            short value;
            std::memcpy(&value, bytes, sizeof(short));
            indices.push_back((GLuint)value);
        }
    }
    return indices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec) {
    std::vector<glm::vec2> vectors;
    for (uint i = 0; i < floatVec.size(); i) {
        vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
    }
    return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec) {
    std::vector<glm::vec3> vectors;
    for (uint i = 0; i < floatVec.size(); i) {
        vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
    }
    return vectors;
}

std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec) {
    std::vector<glm::vec4> vectors;
    for (uint i = 0; i < floatVec.size(); i) {
        vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
    }
    return vectors;
}

std::vector<Texture> Model::getTextures() {
    std::vector<Texture> textures;

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
    }
    return textures;
}

std::vector<Vertex> Model::assembleVertices (
    std::vector<glm::vec3> positions,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec2> textureUVs
) {
    std::vector<Vertex> vertices;
    for (uint i = 0; i < positions.size(); i++) {
        vertices.push_back(
            Vertex {
                positions[i],
                normals[i],
                glm::vec3(1.0f,1.0f,1.0f),
                textureUVs[i]
            }
        );
    }
    return vertices;
}