#include "obj.h"

// Ported from my old MS-DOS 3D Attempt
// TODO: Update to be more C++ conformant. This is disgusting
Mesh* Obj::ObjImport(const char* filename, std::vector <Texture> tex) {
    FILE * fp;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureUVs;
    std::vector<struct faceElement> indices;
    // This is awful
    char str[256];
    fp = fopen(filename, "r");
    // Check if we found the file
	if (fp != NULL) {
		std::cout << filename << " found" << std::endl;
		while (fgets(str, sizeof(str), fp) != NULL){
			if(str[0] == 'v' && str[1] == ' ') {
				/* Load vertices */
                float x,y,z = 0;
				sscanf(str, "v %f %f %f", &x,&y,&z);
                vertices.push_back(glm::vec3(x, y, z));
			} else if(str[0] == 'v' && str[1] == 'n' && str[2] == ' ') {
				/* Load normals */
                float x,y,z = 0;
				sscanf(str, "vn %f %f %f", &x,&y,&z);
                normals.push_back(glm::vec3(x,y,z));
			} else if(str[0] == 'v' && str[1] == 't' && str[2] == ' ') {
				/* Load texture UVs */
                float u,v = 0;
				sscanf(str, "vt %f %f", &u,&v);
                textureUVs.push_back(glm::vec2(u,v));
			}  else if(str[0] == 'f' && str[1] == ' ') {
				/* Load face elements */
                GLuint vx,vy,vz = 0;
                GLuint nx,ny,nz = 0;
                GLuint tx,ty,tz = 0;
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vx,&tx,&nx, &vy,&ty,&ny, &vz,&tz,&nz);
                struct faceElement fx;
                fx.vertex = vx-1;
                fx.normal = nx-1;
                fx.textureUV = tx-1;
                struct faceElement fy;
                fy.vertex = vy-1;
                fy.normal = ny-1;
                fy.textureUV = ty-1;
                struct faceElement fz;
                fz.vertex = vz-1;
                fz.normal = nz-1;
                fz.textureUV = tz-1;
                indices.push_back(fx);
                indices.push_back(fy);
                indices.push_back(fz);
			}
		}
		fclose(fp);
	} else {
		std::cout << "Failed to find " << filename << "!" << std::endl;
		return nullptr;
	}
    // Put the vertices with their normals and Texture UVs
    std::vector<Vertex> vert;
    for (uint i = 0; i < vertices.size(); i++) {
        GLuint normalIndex = 0;
        GLuint texUVindex = 0;
        // Check which normal belongs to which vertex
        for (uint j = 0; j < indices.size(); j++) {
            struct faceElement fe = indices[j];
            if (fe.vertex == i) {
                normalIndex = fe.normal;
                texUVindex = fe.textureUV;
                //std::cout << std::to_string(i) << ": " << std::to_string(texUVindex) << std::endl;
                break;
            }
        }

        vert.push_back(Vertex{vertices[i], glm::vec3(1.0,1.0,1.0), normals[normalIndex], textureUVs[texUVindex]});
    }

    std::vector<GLuint> vertexIndices(indices.size());
    for (size_t i = 0; i < indices.size(); i++) {
        vertexIndices[i] = indices[i].vertex;
    }

    return new Mesh(vert,vertexIndices,tex);
}