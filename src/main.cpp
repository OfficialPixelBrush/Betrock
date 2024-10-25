#include "config.h"

// TODO: Yeet these elsewhere
struct BlockHitResult {
    bool hit;
    glm::ivec3 blockPos;  // The coordinates of the block hit
    glm::vec3 hitPos;     // The exact hit position on the block
    glm::vec3 hitNormal;  // The normal of the face that was hit
};

bool checkIfChunkBoundaryCrossed(glm::vec3 cameraPosition, glm::vec3 previousPosition) {
    // Current chunk coordinates
    int x  = int(floor(cameraPosition.x   / 16.0f)+0.5);
    int z  = int(floor(cameraPosition.z   / 16.0f)+0.5);
    int px = int(floor(previousPosition.x / 16.0f)+0.5);
    int pz = int(floor(previousPosition.z / 16.0f)+0.5);    
    return (px != x || pz != z) ;
}

BlockHitResult raycast(glm::vec3 origin, glm::vec3 direction, float maxDistance, World* world, bool checkForSolidity = false) {
    if (world) {
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
                if (b->getBlockType() != AIR) {
                    if (checkForSolidity) {
                        if (!b->getNonSolid()) {
                            hitPos = origin + direction * rayLength;  // Calculate the exact hit position
                            return {true, currentBlock, hitPos, hitNormal};  // Return block position and hit details
                        }
                    } else {
                        hitPos = origin + direction * rayLength;  // Calculate the exact hit position
                        return {true, currentBlock, hitPos, hitNormal};  // Return block position and hit details
                    }
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
    }

    // No hit within max distance
    return {false, {0, 0, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
}

std::vector<ChunkMesh*> chunkMeshes;
std::vector<DummyMesh> meshBuildQueue;
std::mutex chunkMeshesMutex;
std::mutex meshBuildQueueMutex;
std::mutex chunkRadiusMutex;

void buildChunks(Model* blockModel, World* world, bool& smoothLighting, int& skyLight, std::vector<Chunk*>& toBeUpdated) {
    ChunkBuilder cb(blockModel, world);
    bool building = false;
    bool wasBuilding = false;
    std::cout << "BuildChunk Thread lives!" << std::endl;
    while (true) {
        if (wasBuilding && !building) {
            std::cout << "Finished building Chunks" << std::endl;
            wasBuilding = building;
        }
        if (!toBeUpdated.empty()) {
            building = true;
            // Remove the chunk from the toBeUpdated list
            std::unique_lock<std::mutex> crLock(chunkRadiusMutex);
            Chunk* c = toBeUpdated.front();
            toBeUpdated.erase(toBeUpdated.begin());
            crLock.unlock();

            std::lock_guard<std::mutex> cmLock(chunkMeshesMutex);
            // Iterate over chunkMeshes to find and delete the matching chunk
            for (auto it = chunkMeshes.begin(); it != chunkMeshes.end(); ++it) {
                if (c == (*it)->chunk) {
                    delete *it; // Delete the chunkMesh
                    chunkMeshes.erase(it); // Safely remove it from the vector
                    break;
                }
            }

            // Build a new chunk mesh and add it to the chunkMeshes
            std::lock_guard<std::mutex> mbLock(meshBuildQueueMutex);
            meshBuildQueue.push_back(cb.buildChunk(c, smoothLighting, skyLight));

            // Backwards iteration to remove chunkMeshes with missing chunks
            for (int i = static_cast<int>(chunkMeshes.size()) - 1; i >= 0; --i) {
                Chunk* chunk = world->findChunk(chunkMeshes[i]->chunk->x, chunkMeshes[i]->chunk->z);
                if (!chunk) {
                    delete chunkMeshes[i]; // Delete the chunkMesh
                    chunkMeshes.erase(chunkMeshes.begin() + i); // Erase safely
                }
            }
            wasBuilding = building;
        } else {
            building = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void getChunksInRenderDistance(int renderDistance, int x, int z, World* world, std::vector<Chunk*>& toBeUpdated) {
    std::cout << "Get Chunk Render Distance Thread Called" << std::endl;
    //std::lock_guard<std::mutex> lock(chunkMeshesMutex);
    //std::vector<Chunk*> toBeAdded = 
    world->getChunksInRadius(x,z,renderDistance,toBeUpdated,chunkRadiusMutex);
    /*for (uint i = 0; i < toBeAdded.size(); i++) {
        toBeUpdated.push_back(toBeAdded[i]);
    }
    toBeAdded.clear();*/
}

void updateChunks(Shader& shader, Camera& camera, Sky& sky, int renderDistance, World* world, std::vector<Chunk*>& toBeUpdated) {
    sky.UpdateFog(shader, renderDistance*16);
    int x = int(camera.Position.x);
    int z = int(camera.Position.z);
    std::thread chunkRadiusThread(getChunksInRenderDistance, renderDistance, x, z, world, std::ref(toBeUpdated));
    chunkRadiusThread.detach();
}
Camera* camPointer;

// Callback function to handle window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Adjust the viewport to the new window size
    glViewport(0, 0, width, height);
    std::cout << "Window resized to: " << width << "x" << height << std::endl;
    camPointer->updateResolution(width,height);
}

std::string generateFilename() {
    // Get current time
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    // Format the time into a string (YYYY-MM-DD_HH.MM.SS.png)
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d_%H.%M.%S") << ".png";
    return oss.str();
}

void takeScreenshot(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Allocate memory to store the pixels
    std::vector<unsigned char> pixels(4 * width * height);

    // Read the pixels from the framebuffer
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Flip image vertically and adjust transparency
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 4; ++x) {
            std::swap(pixels[y * width * 4 + x], pixels[(height - 1 - y) * width * 4 + x]);
        }
    }

    // Optionally clear alpha values if needed to make pixels opaque
    for (size_t i = 3; i < pixels.size(); i += 4) {
        if (pixels[i] < 255) { pixels[i] = 255; } // Forces full opacity for each pixel
    }

    // Save the image
    std::string filename = generateFilename();
    if (!stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), width * 4)) {
        std::cerr << "Failed to save screenshot!" << std::endl;
    } else {
        std::cout << "Screenshot saved to " << filename << std::endl;
    }
}

// Targeting OpenGL 3.3
int main(int argc, char *argv[]) {
    // Define a buffer 
    const size_t size = 1024; 
    // Allocate a character array to store the directory path
    char buffer[size];        
    
    // Call _getcwd to get the current working directory and store it in buffer
    if (getcwd(buffer, size) != NULL) {
        // print the current working directory
        std::cout << "Current working directory: " << buffer << std::endl;
    } 
    else {
        // If _getcwd returns NULL, print an error message
        std::cerr << "Error getting current working directory" << std::endl;
    }
    char worldName[256] = "NyareativeMod";
    if (argc < 2) {
        std::cout << "No world name provided!" << std::endl;
        //return 1;
    } else {
        strncpy(worldName,argv[1],sizeof(worldName)-1);
    }
    float fieldOfView = 70.0f;
    int windowWidth = 1280;
    int windowHeight = 720;
    int windowedWidth = windowWidth;
    int windowedHeight = windowHeight;
    int windowedX, windowedY;
    glfwInit();

    // Specify OpenGL Version and Feature-set
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight,"Betrock 0.3.2", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    // Make the window our current OpenGL Context
    glfwMakeContextCurrent(window);

    // Load GLAD so it configure OpenGL
    gladLoadGL();

    // Set the framebuffer size callback to handle resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Define OpenGL Viewport
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0,0,windowWidth,windowHeight);

    // Creates Shader object using shaders default.vsh and .frag
    Shader blockShader("./src/external/shader/default.vsh", "./src/external/shader/minecraft.fsh");
    //Shader skyShader("./src/external/shader/sky.vsh", "./src/external/shader/sky.fsh");
    blockShader.Activate();
    //skyShader.Activate();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Create a camera
    //Camera camera(windowWidth, windowHeight, glm::vec3(20.392706f+0.5, 67.527435f+0.5, 90.234566f+0.5), glm::vec3(0.604827, -0.490525, 0.627354f)); // Glacier Screenshot
    //Camera camera(windowWidth, windowHeight, glm::vec3(-19.11, 66.5, -6.92), glm::vec3(0.0, 0.0, 0.9)); // 404 Screenshot
    Camera camera(windowWidth, windowHeight, glm::vec3(-31.80, 71.73, -55.69), glm::vec3(0.57, 0.05, 0.67)); // Nyareative Screenshot
    //Camera camera(windowWidth, windowHeight, glm::vec3(2.30, 14.62, 235.69), glm::vec3(0.77, -0.32, 0.30)); // Publicbeta Underground Screenshot
    //Camera camera(windowWidth, windowHeight, glm::vec3(47.00, 67.62, 225.59), glm::vec3(0.46, -0.09, 0.76)); // Publicbeta Screenshot
    //Camera camera(windowWidth, windowHeight, glm::vec3(52, 74, 225), glm::vec3(0.0, 0.0, 0.9)); // Testing
    camPointer = &camera;

    // Makes it so OpenGL shows the triangles in the right order
    // Enables the depth buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Face Ordering
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Load Blockmodel
    Model* skyModel = new Model("./src/external/models/sky.obj");
    Model* blockModel = new Model("./src/external/models/models.obj");

    Sky sky(skyModel);

    World* world = new World();

    // ImGui Addition
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool vsync = true;
    bool fullscreen = false;
    bool wasFullscreen = false;
    bool cullFace = true;
    bool polygon = false;
    bool updateWhenMoving = false;
    bool smoothLighting = true;
    bool manualChunkUpdateTrigger = false;
    bool gravity = false;
    bool collision = false;
    bool renderChunks = true;
    bool renderFog = true;
    bool optimalViewDistance = false;
    bool raycastToBlock = true;
    std::vector<Chunk*> toBeUpdated;
    float maxDistance = 100.0f;  // Maximum ray distance (e.g., 100 units)

    double prevTime = glfwGetTime();
    double fpsTime = 0;
    int previousRenderedChunks = 0;
    int maxSkyLight = 15;
    int timeOfDay = 0;
    glm::vec3 previousPosition = camera.Position;

    int renderDistance = 4;

    float x = camera.Position.x;
    float z = camera.Position.z;
    std::string debugText = "";
    std::vector<Texture> tex = blockModel->meshes[0].textures;
    std::thread chunkBuildingThread(buildChunks, std::ref(blockModel), world, std::ref(smoothLighting), std::ref(maxSkyLight), std::ref(toBeUpdated));

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
        glClearColor(sky.skyColor[0],sky.skyColor[1],sky.skyColor[2],sky.skyColor[3]);
        // Clear the Back and Depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ---- INPUTS ----
        if (!io.WantCaptureMouse) {
            camera.Inputs(window);
        }
        // World height is 128 blocks, thus our minimum view distance is 128 units,
        // after 9 chunks, the view distance needs to grow to account for the further away chunks
        if (optimalViewDistance) {
            float maxViewDistance = std::max(128.0f,(float(renderDistance)*16.0f));
            camera.updateMatrix(fieldOfView, 0.01f, maxViewDistance);
        } else {
            camera.updateMatrix(fieldOfView, 0.01f, 10000);
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        debugText = "";

        // ---- RENDERING ----

        // Render Sky
        // Disable Depth Test
        //glDepthFunc(GL_LEQUAL);
        // Bind and render skybox
        //sky.Draw(skyShader, camera);
        // Re-enable Depth Test
        //glDepthFunc(GL_LESS);

        std::unique_lock<std::mutex> cmLock(chunkMeshesMutex, std::try_to_lock);
        std::unique_lock<std::mutex> mbLock(meshBuildQueueMutex, std::try_to_lock);
        // TODO: Fix chunks corrupting
        if (cmLock.owns_lock() && mbLock.owns_lock()) {
            if (!meshBuildQueue.empty()) {
                DummyMesh mesh = meshBuildQueue.back();
                std::vector<Mesh*> meshes;
                if (!mesh.vertices.empty() && !mesh.indices.empty()) {
                    meshes.push_back(new Mesh("world", mesh.vertices, mesh.indices, tex));
                }
                if (!mesh.waterVertices.empty() && !mesh.waterIndices.empty()) {
                    meshes.push_back(new Mesh("water", mesh.waterVertices, mesh.waterIndices, tex));
                }
                ChunkMesh* cm = new ChunkMesh(mesh.chunk, meshes);
                chunkMeshes.push_back(cm);
                meshBuildQueue.pop_back();
            }

            // Sort Chunks with improved precision
            std::sort(chunkMeshes.begin(), chunkMeshes.end(),
                [&camera](const auto& a, const auto& b) {
                    // Only compare if the second mesh is "water" for both chunks
                    if (a->meshes.size() > 1 && a->meshes[1]->name == "water" &&
                        b->meshes.size() > 1 && b->meshes[1]->name == "water") {

                        // Calculate chunk center positions
                        glm::vec2 aChunkPosition = glm::vec2(a->chunk->x * 16 + 8, a->chunk->z * 16 + 8);
                        glm::vec2 bChunkPosition = glm::vec2(b->chunk->x * 16 + 8, b->chunk->z * 16 + 8);
                        glm::vec2 cameraXZ = glm::vec2(camera.Position.x, camera.Position.z);

                        // Calculate squared distances from the camera
                        float distA = glm::length2(aChunkPosition - cameraXZ);
                        float distB = glm::length2(bChunkPosition - cameraXZ);

                        // Primary sorting condition: Compare distances (larger distance comes first)
                        if (std::abs(distA - distB) > 1e-4f) {  // Add epsilon tolerance to avoid precision issues
                            return distA > distB;
                        }

                        // Secondary sorting condition: Compare chunk x positions as tie-breaker
                        if (a->chunk->x != b->chunk->x) {
                            return a->chunk->x > b->chunk->x;
                        }

                        // Final tie-breaker: Compare chunk z positions
                        return a->chunk->z > b->chunk->z;
                    }

                    // If one or both chunks don't have water, maintain original order
                    return false;
                });
        }

        // Render all chunks
        if (renderChunks) {
            for (uint i = 0; i < chunkMeshes.size(); i++) {
                chunkMeshes[i]->Draw(blockShader, camera);
            }
        }

        ImGui::Begin("Options");
        if (ImGui::Button("Screenshot")) {
            takeScreenshot(window);
        }
        ImGui::InputText("##",worldName, 256);
        ImGui::SameLine(0,0);
        if (ImGui::Button("Load") && worldName != "") {
            std::string worldPath = std::string(buffer) + "/saves/" + std::string(worldName) + "/";
            world->chunks.clear();
            world->LoadWorld(worldPath);
            //updateChunks(blockShader, camera, sky, renderDistance, world, toBeUpdated);
        }
        std::string msTime = std::format("Frame time: {:.2f}ms/{:.2f}fps", fpsTime, 1000/fpsTime);
        std::string camPos =  std::format("Position: {:.2f},{:.2f},{:.2f}", camera.Position.x, camera.Position.y,camera.Position.z);
        std::string chunkPos =  "Chunk: " + std::to_string(int(std::floor(camera.Position.x/16))) + ", " + std::to_string(int(std::floor(camera.Position.z/16)));
        std::string camRot =  std::format("Orientation: {:.2f},{:.2f},{:.2f}",camera.Orientation.x,camera.Orientation.y,camera.Orientation.z);
        std::string facing =  "Facing: ";
        // Calculate the angle in radians based on the camera's orientation
        float angle = atan2(camera.Orientation.z, camera.Orientation.x); // Angle in radians

        // Convert radians to degrees (optional, for easier understanding)
        float degrees = angle * 180.0f / M_PI;

        // Determine the direction based on the angle
        // TODO: Fix orientation
        if (angle > -M_PI_4 && angle <= M_PI_4) {
            facing += "East";
        } else if (angle > M_PI_4 && angle <= 3 * M_PI_4) {
            facing += "South";
        } else if (angle > 3 * M_PI_4 || angle <= -3 * M_PI_4) {
            facing += "West";
        } else if (angle > -3 * M_PI_4 && angle <= -M_PI_4) {
            facing += "North";
        }
        std::string camSpeed =  "Speed: " + std::to_string(camera.speed);
        ImGui::Text("%s", msTime.c_str());
        ImGui::Text("%s", camPos.c_str());
        ImGui::Text("%s", chunkPos.c_str());
        ImGui::Text("%s", camRot.c_str());
        ImGui::Text("%s", facing.c_str());
        ImGui::Text("%s", camSpeed.c_str());
        if (ImGui::CollapsingHeader("Colors")) {
            ImGui::ColorEdit4("Sky Color", sky.skyColor);
            ImGui::ColorEdit4("Fog Color", sky.fogColor);
        }
        if (ImGui::CollapsingHeader("Graphical")) {
            ImGui::Checkbox("Vsync", &vsync);
            ImGui::Checkbox("Fullscreen", &fullscreen);
            ImGui::Checkbox("Backface Culling", &cullFace);
            ImGui::Checkbox("Polygon", &polygon);
            ImGui::Checkbox("Optimal View Distance", &optimalViewDistance);
            ImGui::Checkbox("Render Chunks", &renderChunks);
        }

        if (ImGui::CollapsingHeader("Chunks")) {
            ImGui::Checkbox("Smooth Lighting", &smoothLighting);
            ImGui::SliderInt("Skylight",&maxSkyLight, 0, 15);
            std::string renderDistancePreset = "";
            if (renderDistance==16) {
                renderDistancePreset = "Far";
            }
            if (renderDistance==8) {
                renderDistancePreset = "Normal";
            }
            if (renderDistance==4) {
                renderDistancePreset = "Short";
            }
            if (renderDistance==2) {
                renderDistancePreset = "Tiny";
            }
            ImGui::Text("%s", renderDistancePreset.c_str());
            ImGui::SliderInt("Render Distance",&renderDistance, 1, 16);
        
            // Get list of chunks that're to be updated
            if (manualChunkUpdateTrigger || ImGui::Button("Update Chunks") || (checkIfChunkBoundaryCrossed(camera.Position, previousPosition) && updateWhenMoving)) {
                updateChunks(blockShader, camera, sky, renderDistance, world, toBeUpdated);
                manualChunkUpdateTrigger = false;
            }
        }
        if (ImGui::CollapsingHeader("Misc")) {
            int fovTemp = int(fieldOfView/10);
            ImGui::SliderInt("Field of View",&fovTemp, 3, 11);
            fieldOfView = float(fovTemp*10);
            ImGui::Checkbox("Update when Moving", &updateWhenMoving);
            ImGui::Checkbox("Gravity", &gravity);
            ImGui::Checkbox("Collision", &collision);
            ImGui::Checkbox("Raycast Block", &raycastToBlock);
        }

        if (gravity) {
            camera.Position.y -= 0.24;
        }

        if (collision) {
            // Which collisions occured
            bool ground = false;
            bool ceiling = false;
            // Ground
            BlockHitResult groundHit = raycast(camera.Position, glm::vec3(0.0,-1.0,0.0), maxDistance, world, true);
            Block* groundBlock = world->getBlock(groundHit.blockPos.x,groundHit.blockPos.y,groundHit.blockPos.z);

            // Ceiling
            BlockHitResult ceilingHit = raycast(camera.Position, glm::vec3(0.0,1.0,0.0), maxDistance, world, true);
            Block* ceilingBlock = world->getBlock(ceilingHit.blockPos.x,ceilingHit.blockPos.y,ceilingHit.blockPos.z);

            if (groundBlock && camera.Position.y < (groundHit.blockPos.y + 2.8)) {
                //camera.Position = camera.Position-glm::vec3(glm::ivec3(camera.Position)) + glm::vec3(groundHit.blockPos);
                camera.Position.y = groundHit.blockPos.y + 2.8;
                ground = true;
                if (camera.Velocity.y < 0.0) {
                    camera.Velocity.y = 0.0;
                }
            }
            /*
            if (ceilingBlock && camera.Position.y > (ceilingHit.blockPos.y - 0.2)) {
                camera.Position.y = ceilingHit.blockPos.y - 0.2;
                ceiling = true;
                if (camera.Velocity.y < 0.0) {
                    camera.Velocity.y = 0.0;
                }
            }
            if (ground && ceiling) {
                camera.Position = previousPosition;
            }*/

            // North
            BlockHitResult northHit = raycast(camera.Position, glm::vec3(1.0,0.0,0.0), 2, world, true);
            Block* northBlock = world->getBlock(northHit.blockPos.x,northHit.blockPos.y,northHit.blockPos.z);

            // East
            BlockHitResult eastHit = raycast(camera.Position, glm::vec3(0.0,0.0,-1.0), 2, world, true);
            Block* eastBlock = world->getBlock(eastHit.blockPos.x,eastHit.blockPos.y,eastHit.blockPos.z);

            // South
            BlockHitResult southHit = raycast(camera.Position, glm::vec3(-1.0,0.0,0.0), 2, world, true);
            Block* southBlock = world->getBlock(southHit.blockPos.x,southHit.blockPos.y,southHit.blockPos.z);

            // West
            BlockHitResult westHit = raycast(camera.Position, glm::vec3(0.0,0.0,1.0), 2, world, true);
            Block* westBlock = world->getBlock(westHit.blockPos.x,westHit.blockPos.y,westHit.blockPos.z);

            if (northBlock && camera.Position.x > northHit.blockPos.x - 0.2) {
                if (camera.Velocity.x < 0.0) {
                    camera.Velocity.x = 0.0;
                }
                camera.Position.x = previousPosition.x;
            }

            if (southBlock && camera.Position.x < southHit.blockPos.x + 1.2) {
                if (camera.Velocity.x > 0.0) {
                    camera.Velocity.x = 0.0;
                }
                camera.Position.x = previousPosition.x;
            }

            if (eastBlock && camera.Position.z < eastHit.blockPos.z + 1.2) {
                if (camera.Velocity.z > 0.0) {
                    camera.Velocity.z = 0.0;
                }
                camera.Position.z = previousPosition.z;
            }

            if (westBlock && camera.Position.z > westHit.blockPos.z - 0.2) {
                if (camera.Velocity.z < 0.0) {
                    camera.Velocity.z = 0.0;
                }
                camera.Position.z = previousPosition.z;
            }
            
            //camera.Speed = 4.317;
        }

        if (fullscreen && !wasFullscreen) {
            // Get the primary monitor and its video mode
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            // Save windowed mode position and size
            glfwGetWindowPos(window, &windowedX, &windowedY);
            glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

            // Set the window to fullscreen on the primary monitor
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else if (wasFullscreen && !fullscreen) {
            // Restore the window to windowed mode
            glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
        }

        if (raycastToBlock) {
            // Debug Raycast to looked-at Block
            BlockHitResult hit = raycast(camera.Position, camera.Orientation, maxDistance, world);

            if (!hit.hit) {
                debugText += "No block hit.\n";
            } else {
                debugText +=  "Hit block at: " + std::to_string(hit.blockPos.x) + ", " + std::to_string(hit.blockPos.y) + ", " + std::to_string(hit.blockPos.z) + "\n";
                Block* b = world->getBlock(hit.blockPos.x,hit.blockPos.y,hit.blockPos.z);
                if (b) {
                    debugText += "Name: " + b->getName() + "\n";
                    debugText += "Id: " + std::to_string(b->blockType) + "\n";
                    debugText += "MetaData: " + std::to_string(b->metaData) + "\n";
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
        } else {
            Block* b = world->getBlock(floor(camera.Position.x),floor(camera.Position.y),floor(camera.Position.z));
            if (b) {
                debugText += "Name: " + b->getName() + "\n";
                debugText += "Id: " + std::to_string(b->blockType) + "\n";
                debugText += "MetaData: " + std::to_string(b->metaData) + "\n";
                debugText += "Transparent: " + std::to_string(b->transparent) + "\n";
                debugText += "LightSource: " + std::to_string(b->lightSource) + "\n";
                debugText += "PartialBlock: " + std::to_string(b->partialBlock) + "\n";
            }
        }
        ImGui::SeparatorText("Debug");
        ImGui::Text("%s", debugText.c_str());
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap the back and front buffer
        glfwSwapBuffers(window);
        // Respond to all GLFW events
        glfwPollEvents();
        fpsTime = (glfwGetTime() - prevTime)*1000;
        prevTime = glfwGetTime();
        camera.setDelta(fpsTime);
        previousPosition = camera.Position;
        if (world) {
            previousRenderedChunks = world->chunks.size();
        }
        wasFullscreen = fullscreen;
     }

    // Clean-up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}