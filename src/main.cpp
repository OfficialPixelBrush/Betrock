#include "config.h"

float skyColor [] = {0.439f, 0.651f, 0.918f, 1.0f};

bool checkIfChunkBoundaryCrossed(glm::vec3 cameraPosition, glm::vec3 previousPosition) {
    // Current chunk coordinates
    int x = floor(cameraPosition.x / 16.0f);
    int z = floor(cameraPosition.z / 16.0f);
    int px = floor(previousPosition.x / 16.0f);
    int pz = floor(previousPosition.z / 16.0f);    
    return (px != x || pz != z) ;
}
#include <glm/glm.hpp>
#include <cmath>
#include <limits>

struct BlockHitResult {
    bool hit;
    glm::ivec3 blockPos;  // The coordinates of the block hit
    glm::vec3 hitPos;     // The exact hit position on the block
    glm::vec3 hitNormal;  // The normal of the face that was hit
};

BlockHitResult raycast(glm::vec3 origin, glm::vec3 direction, float maxDistance, World* world) {
    glm::ivec3 currentBlock = glm::floor(origin);  // Start from the block containing the origin

    glm::vec3 deltaDist = glm::abs(glm::vec3(1.0f) / direction);  // How far to move in each axis
    glm::ivec3 step;
    glm::vec3 sideDist;

    // Determine step direction and initial side distances, taking the fractional origin into account
    for (int i = 0; i < 3; i++) {
        if (direction[i] < 0) {
            step[i] = -1;
            sideDist[i] = (origin[i] - currentBlock[i]) * deltaDist[i];  // Fractional part considered
        } else {
            step[i] = 1;
            sideDist[i] = (currentBlock[i] + 1.0f - origin[i]) * deltaDist[i];  // Fractional part considered
        }
    }

    float rayLength = 0.0f;
    glm::vec3 hitPos;
    glm::vec3 hitNormal(0.0f);  // Used to store the face normal that the ray hits

    while (rayLength < maxDistance) {
        // Check if the current block contains something
        Block* b = world->getBlock(currentBlock.x, currentBlock.y, currentBlock.z);
        if (b != nullptr) {
            // We can't hit air!
            if (b->getBlockType() != 0) {
                hitPos = origin + direction * rayLength;  // Calculate the exact hit position
                return {true, currentBlock, hitPos, hitNormal};  // Return block position and hit details
            }
        }

        // Move to the next block, keeping track of the face the ray crosses
        if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
            currentBlock.x += step.x;
            rayLength = sideDist.x;
            sideDist.x += deltaDist.x;
            hitNormal = glm::vec3(-step.x, 0.0f, 0.0f);  // Crossing an x face
        } else if (sideDist.y < sideDist.z) {
            currentBlock.y += step.y;
            rayLength = sideDist.y;
            sideDist.y += deltaDist.y;
            hitNormal = glm::vec3(0.0f, -step.y, 0.0f);  // Crossing a y face
        } else {
            currentBlock.z += step.z;
            rayLength = sideDist.z;
            sideDist.z += deltaDist.z;
            hitNormal = glm::vec3(0.0f, 0.0f, -step.z);  // Crossing a z face
        }
    }

    // No hit within max distance
    return {false, {0, 0, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
}


// Targeting OpenGL 3.3
int main(int argc, char *argv[]) {
    std::string worldName;
    if (argc < 2) {
        std::cout << "No world name provided!" << std::endl;
        worldName = "publicbeta";
        //return 1;
    } else {
        worldName = argv[1];
    }
    float fieldOfView = 70.0f;
    int windowWidth = 1280;
    int windowHeight = 720;
    glfwInit();

    // Specify OpenGL Version and Feature-set
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight,"Betrock 0.2.3", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    // Make the window our current OpenGL Context
    glfwMakeContextCurrent(window);

    // Load GLAD so it configure OpenGL
    gladLoadGL();

    // Define OpenGL Viewport
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0,0,windowWidth,windowHeight);

    // Creates Shader object using shaders default.vsh and .frag
    Shader shaderProgram("../src/shader/default.vsh", "../src/shader/minecraft.fsh");
    shaderProgram.Activate();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Create a camera at 0,0,2
    //Camera camera(windowWidth, windowHeight, glm::vec3(20.392706f, 67.527435f, 90.234566f), glm::vec3(0.604827, -0.490525, 0.627354f));
    //Camera camera(windowWidth, windowHeight, glm::vec3(-12, 65.18, 0.88), glm::vec3(0.0, 0.0, 0.9));    // Draw Clear Color
    Camera camera(windowWidth, windowHeight, glm::vec3(52, 74, 222), glm::vec3(0.0, 0.0, 0.9));
    glClearColor(skyColor[0],skyColor[1],skyColor[2],skyColor[3]);

    // Makes it so OpenGL shows the triangles in the right order
    // Enables the depth buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Face Ordering
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Load Blockmodel
    Model blockModel("models/models.obj");

    worldName = "saves/" + worldName;
    World* world = new World(worldName);

    ChunkBuilder cb(&blockModel, world);

    // ImGui Addition
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool vsync = true;
    bool cullFace = true;
    bool polygon = false;
    bool updateWhenMoving = false;
    bool smoothLighting = true;
    bool manualChunkUpdateTrigger = true;

    double prevTime = glfwGetTime();
    double fpsTime = 0;
    int previousRenderedChunks = 0;
    int maxSkyLight = 15;
    glm::vec3 previousPosition = camera.Position;

    int renderDistance = 3;

    float x = camera.Position.x;
    float z = camera.Position.z;
    std::vector<ChunkMesh*> chunkMeshes;
    std::string debugText = "Debug Text:\n";

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        // Update Options
        // Toggle Vsync
        if (vsync) {
            glfwSwapInterval( 1 );
        } else {
            glfwSwapInterval( 0 );
        }

        // Toggle Backface culling
        if (cullFace) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        
        // Toggle Wireframe drawing
        if (polygon) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        } else {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }

        // Draw
        glClearColor(skyColor[0],skyColor[1],skyColor[2],skyColor[3]);
        // Clear the Back and Depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handle inputs
        if (!io.WantCaptureMouse) {
            camera.Inputs(window);
        }
        camera.updateMatrix(fieldOfView, 0.1f, 300.0f);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Sort Chunks
        std::sort(chunkMeshes.begin(), chunkMeshes.end(),
        [&camera](const auto& a, const auto& b) {
            if (a->meshes[1]->name == "water") {
                // Assuming each chunkMesh has a method getPosition() that returns a glm::vec3
                glm::vec2 aChunkPosition = glm::vec2(a->chunk->x*16+8,a->chunk->z*16+8);
                glm::vec2 bChunkPosition = glm::vec2(b->chunk->x*16+8,b->chunk->z*16+8);
                glm::vec2 cameraXZ = glm::vec2(camera.Position.x,camera.Position.z);
                float distA = glm::length2(aChunkPosition - cameraXZ);
                float distB = glm::length2(bChunkPosition - cameraXZ);
                return distA > distB;
            } else {
                return false;
            }
        });

        for (uint i = 0; i < chunkMeshes.size(); i++) {
            chunkMeshes[i]->Draw(shaderProgram, camera);
        }

        ImGui::Begin("Options");
        std::string msTime =  "Frame time: " + std::to_string(fpsTime) + "ms/" + std::to_string(1000/fpsTime) + "fps";
        std::string camPos =  "Position: " + std::to_string(camera.Position.x) + ", " + std::to_string(camera.Position.y) + ", " + std::to_string(camera.Position.z);
        std::string chunkPos =  "Chunk: " + std::to_string(int(std::floor(camera.Position.x/16))) + ", " + std::to_string(int(std::floor(camera.Position.z/16)));
        std::string camRot =  "Orientation: " + std::to_string(camera.Orientation.x) + ", " + std::to_string(camera.Orientation.y) + ", " + std::to_string(camera.Orientation.z);
        std::string camSpeed =  "Speed: " + std::to_string(camera.speed);
        ImGui::Text(msTime.c_str());
        ImGui::Text(camPos.c_str());
        ImGui::Text(chunkPos.c_str());
        ImGui::Text(camRot.c_str());
        ImGui::Text(camSpeed.c_str());
        ImGui::ColorEdit4("Sky Color", skyColor);
        ImGui::Checkbox("Vsync", &vsync);
        ImGui::Checkbox("Backface Culling", &cullFace);
        ImGui::Checkbox("Polygon", &polygon);
        ImGui::Checkbox("Update when Moving", &updateWhenMoving);
        ImGui::Checkbox("Smooth Lighting", &smoothLighting);
        ImGui::SliderInt("Skylight",&maxSkyLight, 0, 15);
        ImGui::SliderInt("Render Distance",&renderDistance, 1, 16);
        //if (ImGui::Button("Get Block Info")) {
            float maxDistance = 100.0f;  // Maximum ray distance (e.g., 100 units)
            BlockHitResult hit = raycast(camera.Position, camera.Orientation, maxDistance, world);
            debugText = "Debug Text:\n";

            if (!hit.hit) {
                debugText += "No block hit.\n";
            } else {
                debugText +=  "Hit block at: " + std::to_string(hit.blockPos.x) + ", " + std::to_string(hit.blockPos.y) + ", " + std::to_string(hit.blockPos.z) + "\n";
                Block* b = world->getBlock(hit.blockPos.x,hit.blockPos.y,hit.blockPos.z);
                if (b) {
                    debugText += "Name: " + b->getName() + "\n";
                    debugText += "Id: " + std::to_string(b->blockType) + "\n";
                    debugText += "MetaData: " + std::to_string(b->metaData) + "\n";
                    debugText += "Facing: " + b->getFacing() + "\n";
                    debugText += "Transparent: " + std::to_string(b->transparent) + "\n";
                    debugText += "LightSource: " + std::to_string(b->lightSource) + "\n";
                    debugText += "PartialBlock: " + std::to_string(b->partialBlock) + "\n";
                }
                Block* bn = world->getBlock(hit.blockPos.x+hit.hitNormal.x,hit.blockPos.y+hit.hitNormal.y,hit.blockPos.z+hit.hitNormal.z);
                if (bn) {
                    debugText += "SkyLight: " + std::to_string(bn->skyLightLevel) + "\n";
                    debugText += "LightLevel: " + std::to_string(bn->lightLevel) + "\n";
                }
            }
        //}
        if (ImGui::Button("Clear Chunks")) {
            // Clear pointers in world->chunks if needed
            for (Chunk* chunk : world->chunks) {
                delete chunk;  // Delete each chunk pointer to avoid memory leaks
            }
            world->chunks.clear();  // Now clear the vector

            // Clear chunkMeshes
            for (uint i = 0; i < chunkMeshes.size(); i++) {
                delete chunkMeshes[i];  // Delete each mesh
            }
            chunkMeshes.clear();  // Clear the vector

            manualChunkUpdateTrigger = true;
        }
        if (manualChunkUpdateTrigger || ImGui::Button("Update Chunks") || (checkIfChunkBoundaryCrossed(camera.Position, previousPosition) && updateWhenMoving)) {
            float x = camera.Position.x;
            float z = camera.Position.z;
            std::vector<Chunk*> toBeUpdated = world->getChunksInRadius(int(x),int(z),renderDistance);
            std::vector<ChunkMesh*> newChunkMeshes = cb.buildChunks(toBeUpdated,smoothLighting,maxSkyLight);

            // Delete existing chunks from chunkmesh if they're part of the new chunks
            for (auto it = chunkMeshes.begin(); it != chunkMeshes.end();) {
                Chunk* existingChunk = (*it)->chunk;

                // Check if the existing chunk is in the newChunkMeshes
                bool found = false;
                for (const auto& newChunkMesh : newChunkMeshes) {
                    if (newChunkMesh->chunk == existingChunk) {
                        found = true;
                        break;
                    }
                }
                // If found, delete the existing chunk and remove it from chunkMeshes
                if (found) {
                    delete *it;  // Free memory of the existing chunk mesh
                    it = chunkMeshes.erase(it);  // Remove from the vector and advance iterator
                } else {
                    ++it;  // Advance the iterator without deleting
                }
                manualChunkUpdateTrigger = false;
            }

            // Add new chunks to chunkmesh
            for (uint i = 0; i < newChunkMeshes.size(); i++) {
                chunkMeshes.push_back(newChunkMeshes[i]);
            }

            newChunkMeshes.clear();
            for (uint i = chunkMeshes.size()-1; i > 0; i--) {
                Chunk* chunk = world->findChunk(chunkMeshes[i]->chunk->x,chunkMeshes[i]->chunk->z);
                if (!chunk) {
                    delete chunkMeshes[i];
                    chunkMeshes.erase(chunkMeshes.begin() + i);
                }
            }
        }
        ImGui::Text(debugText.c_str());
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap the back and front buffer
        glfwSwapBuffers(window);
        // Respond to all GLFW events
        glfwPollEvents();
        fpsTime = (glfwGetTime() - prevTime)*1000;
        prevTime = glfwGetTime();
        previousPosition = camera.Position;
        previousRenderedChunks = world->chunks.size();
     }

    // Clean-up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}