#pragma once
#include "../render/mesh.h"
#include "../render/texture.h"

struct faceElement {
    GLuint vertex;
    GLuint normal;
    GLuint textureUV;
};

class Obj {
    public:
        Mesh* ObjImport(const char* filename, std::vector <Texture> tex);
};