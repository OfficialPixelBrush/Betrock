#include "../render/mesh.h"
#include "../render/camera.h"
#include "../world/chunk.h"
#include <vector>

class ChunkMesh {
    // Mesh 0 is terrain
    // Mesh 1 is water/semi-transparent
        public:
            std::vector<Mesh*> meshes;
            Chunk* chunk;
            ChunkMesh(Chunk* chunk, std::vector<Mesh*> meshes) {
                this->chunk = chunk;  // Move the unique_ptr to transfer ownership
                this->meshes = meshes; // Same for meshes if you're moving them as well
                std::cout << "Created Chunk Mesh " << chunk->x << ", " << chunk-> z << std::endl;
            }
            ~ChunkMesh() {
                for (Mesh* mesh : meshes) {
                    delete mesh;  // Free memory for each Mesh object
                }
                std::cout << "Deleted Chunk Mesh " << chunk->x << ", " << chunk-> z << std::endl;
            }
            void Remesh();
            void Draw(Shader& shader, Camera& camera) {
                for (const auto& m : meshes) {
                    m->Draw(shader,camera);
                }
            };
};