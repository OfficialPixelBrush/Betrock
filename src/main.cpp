#include "main.h"

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
                if (b->blockType != AIR) {
                    if (checkForSolidity) {
                        if (!isNonSolid(b->blockType)) {
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
Camera* camPointer = nullptr;

void buildChunks(Model* blockModel, World* world, bool& smoothLighting, int& skyLight, bool& solidTrees, std::vector<Chunk*>& toBeUpdated) {
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

            /*
            // Iterate over chunkMeshes to find and delete the matching chunk
            for (auto it = chunkMeshes.begin(); it != chunkMeshes.end(); ++it) {
                if (c == (*it)->chunk) {
                    delete *it; // Delete the chunkMesh
                    chunkMeshes.erase(it); // Safely remove it from the vector
                    break;
                }
            }*/

            // Build a new chunk mesh and add it to the chunkMeshes
            std::unique_lock<std::mutex> mbLock(meshBuildQueueMutex);
            meshBuildQueue.push_back(cb.buildChunk(c, smoothLighting, solidTrees, skyLight));
            mbLock.unlock();

            // Backwards iteration to remove chunkMeshes with missing chunks
            std::unique_lock<std::mutex> cmLock(chunkMeshesMutex, std::try_to_lock);
            if (cmLock.owns_lock()) {
                for (int i = static_cast<int>(chunkMeshes.size()) - 1; i >= 0; --i) {
                    Chunk* chunk = world->findChunk(chunkMeshes[i]->chunk->x, chunkMeshes[i]->chunk->z);
                    if (!chunk) {
                        delete chunkMeshes[i]; // Delete the chunkMesh
                        chunkMeshes.erase(chunkMeshes.begin() + i); // Erase safely
                    }
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

void updateChunks(Sky& sky, int renderDistance, World* world, std::vector<Chunk*>& toBeUpdated) {
    int x = int(camPointer->Position.x);
    int z = int(camPointer->Position.z);
    std::thread chunkRadiusThread(getChunksInRenderDistance, renderDistance, x, z, world, std::ref(toBeUpdated));
    chunkRadiusThread.detach();
}

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


// Debug callback function
void APIENTRY OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                         GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam)
{
    std::cerr << "OpenGL Debug Message:\n";
    std::cerr << "  Source: " << source << "\n";
    std::cerr << "  Type: " << type << "\n";
    std::cerr << "  ID: " << id << "\n";
    std::cerr << "  Severity: " << severity << "\n";
    std::cerr << "  Message: " << message << "\n\n";
    int x;
    //std::cin >> x;
}

void EnableOpenGLDebug()
{
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLDebugMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    else
    {
        std::cerr << "Warning: OpenGL debug context not available.\n";
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
    char worldName[256] = "publicbeta";
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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight,PROJECT_NAME_VERSION, NULL, NULL);
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
    Shader blockShader("./shaders/default.vsh", "./shaders/minecraft.fsh");
    Shader normalShader("./shaders/default.vsh", "./shaders/normal.fsh");
    Shader defaultShader("./shaders/default.vsh", "./shaders/default.fsh");
    Shader skyShader("./shaders/sky.vsh", "./shaders/sky.fsh");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    EnableOpenGLDebug();

    // Create a camera
    //camPointer = new Camera(windowWidth, windowHeight, glm::vec3(20.392706f+0.5, 67.527435f+0.5, 90.234566f+0.5), glm::vec3(0.604827, -0.490525, 0.627354f)); // Glacier Screenshot
    //camPointer = new Camera(windowWidth, windowHeight, glm::vec3(-19.11, 66.5, -6.92), glm::vec3(0.0, 0.0, 0.9)); // 404 Screenshot
    //camPointer = new Camera(windowWidth, windowHeight, glm::vec3(-31.80, 71.73, -55.69), glm::vec3(0.57, 0.05, 0.67)); // Nyareative Screenshot
    //camPointer = new Camera(windowWidth, windowHeight, glm::vec3(-18.77, 70.60, -42.00), glm::vec3(0.13, -0.79, 0.36)); // Nyareative Chunk Error
    //camPointer = new Camera(windowWidth, windowHeight, glm::vec3(2.30, 14.62, 235.69), glm::vec3(0.77, -0.32, -0.30)); // Publicbeta Underground Screenshot
    //camPointer = new Camera(windowWidth, windowHeight, glm::vec3(47.00, 67.62, 225.59), glm::vec3(0.46, -0.09, 0.76)); // Publicbeta Screenshot
    //camPointer = new Camera(windowWidth, windowHeight, glm::vec3(59.76, 67.41, 251.58), glm::vec3(-0.63, 0.13, -0.61)); // Publicbeta Bg, Fov 50
    camPointer = new Camera(windowWidth, windowHeight, glm::vec3(0, 90, 0), glm::vec3(0.67, -0.57, -0.13)); // Testing

    // Makes it so OpenGL shows the triangles in the right order
    // Enables the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Face Ordering
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Initialize Block Property LUT
    initializeBlockLUTs();

    // Load Blockmodel
    Model* blockModel = new Model("./models/models.obj");
    Model* skyModel = new Model("./models/sky.obj");

    Sky sky(skyModel->meshes[0].get());

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
    bool wireframe = false;
    bool updateWhenMoving = false;
    bool smoothLighting = true;
    bool manualChunkUpdateTrigger = false;
    bool gravity = false;
    bool collision = false;
    bool renderChunks = true;
    bool renderFog = true;
    bool optimalViewDistance = false;
    bool raycastToBlock = true;
    bool normals = false;
    bool fullBright = false;
    bool fogEnabled = true;
    bool waterSorting = true;
    bool solidTrees = true;
    std::vector<Chunk*> toBeUpdated;
    float maxDistance = 100.0f;  // Maximum ray distance (e.g., 100 units)

    double prevTime = glfwGetTime();
    double fpsTime = 0;
    int previousRenderedChunks = 0;
    int maxSkyLight = 15;
    int timeOfDay = 0;
    glm::vec3 previousPosition = camPointer->Position;

    int renderDistance = 8;
    sky.UpdateFog(blockShader, renderDistance*16);

    float x = camPointer->Position.x;
    float z = camPointer->Position.z;
    std::string debugText = "";
    std::vector<Texture> tex = blockModel->meshes[0]->textures;
    std::thread chunkBuildingThread(buildChunks, std::ref(blockModel), world, std::ref(smoothLighting), std::ref(maxSkyLight), std::ref(solidTrees), std::ref(toBeUpdated));
    //std::this_thread::sleep_for(std::chrono::milliseconds(25));
    //std::thread chunkBuildingThread2(buildChunks, std::ref(blockModel), world, std::ref(smoothLighting), std::ref(maxSkyLight), std::ref(toBeUpdated));

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        // Update Options
        // Toggle Vsync
        if (vsync) {
            glfwSwapInterval( 1 );
        } else {
            glfwSwapInterval( 0 );
        }

        // Draw
        float skyLightMultiplier = float(maxSkyLight)/15.0;
        glClearColor(
            sky.skyColor[0]*skyLightMultiplier,
            sky.skyColor[1]*skyLightMultiplier,
            sky.skyColor[2]*skyLightMultiplier,
            sky.skyColor[3]*skyLightMultiplier
        );
        // Clear the Back and Depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ---- INPUTS ----
        if (!io.WantCaptureMouse) {
            camPointer->Inputs(window);
        }
        // World height is 128 blocks, thus our minimum view distance is 128 units,
        // after 9 chunks, the view distance needs to grow to account for the further away chunks
        if (optimalViewDistance) {
            float maxViewDistance = std::max(128.0f,(float(renderDistance)*16.0f));
            camPointer->updateMatrix(fieldOfView, 0.01f, maxViewDistance);
        } else {
            camPointer->updateMatrix(fieldOfView, 0.01f, 10000);
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        debugText = "";

        // ---- RENDERING ----

        // Render Skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_ALWAYS);
        glDisable(GL_CULL_FACE);
        skyShader.Activate();
        skyShader.setMat4("projection", camPointer->GetProjectionMatrix());
        skyShader.setMat4("view", glm::mat4(glm::mat3(camPointer->GetViewMatrix())));
        skyShader.setFloat("timeOfDay", float(maxSkyLight)/15.0);
        sky.Draw(skyShader, *camPointer);
        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);

        // Toggle Backface culling
        if (cullFace) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        
        // Toggle Wireframe drawing
        if (wireframe) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        } else {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }

        // Re-enable Depth Test
        std::unique_lock<std::mutex> mbLock(meshBuildQueueMutex, std::try_to_lock);
        // FIXED: Fix chunks corrupting
        // Turns out chunks corrupted due to non-triangulated meshes!!!
        if (mbLock.owns_lock()) {
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
        }

        // Sort Chunks by Manhattan Distance
        // Maybe add  && checkIfChunkBoundaryCrossed(camPointer->Position, previousPosition)?
        if (waterSorting) {
            std::unique_lock<std::mutex> cmLock(chunkMeshesMutex);
            std::sort(chunkMeshes.begin(), chunkMeshes.end(),
                [camPointer](const auto& a, const auto& b) {
                    // Only compare if the second mesh is "water" for both chunks
                    if (a->meshes.size() > 1 && a->meshes[1]->name == "water" &&
                        b->meshes.size() > 1 && b->meshes[1]->name == "water") {

                        // Calculate chunk center positions
                        glm::vec2 aChunkPosition = glm::vec2(a->chunk->x * 16 + 8, a->chunk->z * 16 + 8);
                        glm::vec2 bChunkPosition = glm::vec2(b->chunk->x * 16 + 8, b->chunk->z * 16 + 8);
                        glm::vec2 cameraXZ = glm::vec2(camPointer->Position.x, camPointer->Position.z);

                        // Calculate Manhattan distances from the camera
                        float distA = std::abs(aChunkPosition.x - cameraXZ.x) + std::abs(aChunkPosition.y - cameraXZ.y);
                        float distB = std::abs(bChunkPosition.x - cameraXZ.x) + std::abs(bChunkPosition.y - cameraXZ.y);

                        // Primary sorting condition: Compare distances (larger distance comes first)
                        if (std::abs(distA - distB) > 1e-4f) {  // Add epsilon tolerance to avoid precision issues
                            return distA > distB;
                        }
                    }
                    return false;
                });
            cmLock.unlock();
        }
        // Render all chunks
        // TODO: Only render chunks that're actually visible
        if (renderChunks) {
            blockShader.Activate();
            blockShader.setFloat("maxSkyLight", float(maxSkyLight));
            blockShader.setBool("fullbright", fullBright);
            blockShader.setBool("fogEnabled",fogEnabled);
            // Camera Vector
            glm::vec2 ap = 
                glm::vec2(sin(camPointer->Orientation.x), -cos(camPointer->Orientation.x));
            ap = glm::normalize(ap);
            for (uint i = 0; i < chunkMeshes.size(); i++) {
                if (!chunkMeshes[i]->chunk) {
                    continue;
                }
                /*
                // Camera Pos to Chunk Vector
                glm::vec2 ab =
                    glm::vec2(x - float(chunkMeshes[i]->chunk->x)*16.0f, z - float(chunkMeshes[i]->chunk->z)*16.0f);
                ab = glm::normalize(ab);
                std::cout << "CamRaw: " << camPointer->Orientation.x << ", " << camPointer->Orientation.x << std::endl;
                std::cout << "Chunk : " << ab.x << ", " << ab.y << std::endl;
                std::cout << "Cam   : " << ap.x << ", " << ap.y << std::endl;
                // Make it so only chunks that're facing the camera can be seen
                if (glm::dot(ap,ab) <= 0.0f) {
                    continue;
                }*/
                
                if (normals) {
                    chunkMeshes[i]->Draw(normalShader, *camPointer);
                } else {
                    chunkMeshes[i]->Draw(blockShader, *camPointer);
                }
            }
        }

        ImGui::Begin("Options");
        {
            if (ImGui::Button("Screenshot")) {
                takeScreenshot(window);
            }
            ImGui::InputText("##",worldName, 256);
            ImGui::SameLine(0,0);
            if (ImGui::Button("Load") && worldName != "") {
                std::string worldPath = std::string(buffer) + "/saves/" + std::string(worldName) + "/";
                world->clearChunks();
                world->LoadWorld(worldPath);
                updateChunks(sky, renderDistance, world, toBeUpdated);
            }
            std::string msTime = fmt::v9::format("Frame time: {:.2f}ms/{:.2f}fps", fpsTime, 1000/fpsTime);
            std::string camPos =  fmt::v9::format("Position: {:.2f},{:.2f},{:.2f}", camPointer->Position.x, camPointer->Position.y,camPointer->Position.z);
            std::string chunkPos =  "Chunk: " + std::to_string(int(std::floor(camPointer->Position.x/16.0f))) + ", " + std::to_string(int(std::floor(camPointer->Position.z/16.0f)));
            Chunk* currentChunk = world->findChunk(int(std::floor(camPointer->Position.x/16.0f)),int(std::floor(camPointer->Position.z/16.0f)));
            if (currentChunk) {
                if (currentChunk->populated > 0) {
                    chunkPos += " (Populated)";
                } else {
                    chunkPos += " (Generated)";
                }
            }
            std::string camRot =  fmt::v9::format("Orientation: {:.2f},{:.2f},{:.2f}",camPointer->Orientation.x,camPointer->Orientation.y,camPointer->Orientation.z);
            std::string facing =  "Facing: ";
            // Calculate the angle in radians based on the camera's orientation
            float angle = atan2(camPointer->Orientation.z, camPointer->Orientation.x); // Angle in radians

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
            std::string camSpeed =  "Speed: " + std::to_string(camPointer->speed);
            ImGui::Text("%s", msTime.c_str());
            ImGui::Text("%s", camPos.c_str());
            ImGui::Text("%s", chunkPos.c_str());
            ImGui::Text("%s", camRot.c_str());
            ImGui::Text("%s", facing.c_str());
            ImGui::Text("%s", camSpeed.c_str());
            static float tpCoords[3] = { camPointer->Position.x, camPointer->Position.y, camPointer->Position.z };
            ImGui::InputFloat3("TP Coords", tpCoords);
            if (ImGui::Button("Teleport")) {
                camPointer->Position = glm::vec3(tpCoords[0], tpCoords[1], tpCoords[2]);
            }
            if (ImGui::CollapsingHeader("Colors")) {
                ImGui::ColorEdit4("Sky Color", sky.skyColor);
                ImGui::ColorEdit4("Fog Color", sky.fogColor);
            }
            if (ImGui::CollapsingHeader("Graphical")) {
                ImGui::Checkbox("Vsync", &vsync);
                ImGui::Checkbox("Fullscreen", &fullscreen);
                ImGui::Checkbox("Backface Culling", &cullFace);
                ImGui::Checkbox("Wireframe", &wireframe);
                ImGui::Checkbox("Water Sorting", &waterSorting);
                ImGui::Checkbox("Fullbright", &fullBright);
                ImGui::Checkbox("Solid Trees", &solidTrees);
                ImGui::Checkbox("Optimal View Distance", &optimalViewDistance);
                ImGui::Checkbox("Render Chunks", &renderChunks);
                ImGui::Checkbox("Fog", &fogEnabled);
                ImGui::Checkbox("Normals", &normals);
            }

            if (ImGui::CollapsingHeader("Chunks")) {
                ImGui::Checkbox("Smooth Lighting", &smoothLighting);
                ImGui::SliderInt("Skylight",&maxSkyLight, 0, 15);
                std::string renderDistancePreset = "";
                switch(renderDistance) {
                    case 32:
                        renderDistancePreset = "Extreme";
                        break;
                    case 16: 
                        renderDistancePreset = "Far";
                        break;
                    case 8:
                        renderDistancePreset = "Normal";
                        break;
                    case 4:
                        renderDistancePreset = "Short";
                        break;
                    case 2:
                        renderDistancePreset = "Tiny";
                        break;
                }
                ImGui::Text("%s", renderDistancePreset.c_str());
                if (ImGui::SliderInt("Render Distance",&renderDistance, 1, 32)) {
                    sky.UpdateFog(blockShader, renderDistance*16);
                }
            
                // Get list of chunks that're to be updated
                if (manualChunkUpdateTrigger || ImGui::Button("Update Chunks") || (checkIfChunkBoundaryCrossed(camPointer->Position, previousPosition) && updateWhenMoving)) {
                    updateChunks(sky, renderDistance, world, toBeUpdated);
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
            if (ImGui::CollapsingHeader("Debug Flags")) {
                ImGui::Checkbox("nbtDebug", &nbtDebug);
                ImGui::Checkbox("regionLoaderDebug", &regionLoaderDebug);
                ImGui::Checkbox("chunkBuilderDebug", &chunkBuilderDebug);
                ImGui::Checkbox("logoDebug", &logoDebug);
                if (ImGui::Button("Tp to logo")) {
                    fieldOfView = 50.0;
                    camPointer->Position = glm::vec3(-7.0, 18.0, 14);
                    camPointer->Orientation = glm::vec3(0.40, -0.9, 0.0);
                }
            }

            if (collision) {
                // Which collisions occured
                bool ground = false;
                bool ceiling = false;
                Block* groundBlock = nullptr;
                Block* ceilingBlock = nullptr;
                // Ground
                BlockHitResult groundHit = raycast(camPointer->Position, glm::vec3(0.0,-1.0,0.0), maxDistance, world, true);
                if (groundHit.hit) {
                    groundBlock = world->getBlock(groundHit.blockPos.x,groundHit.blockPos.y,groundHit.blockPos.z);
                }

                // Ceiling
                BlockHitResult ceilingHit = raycast(camPointer->Position, glm::vec3(0.0,1.0,0.0), maxDistance, world, true);
                if (ceilingHit.hit) {
                    ceilingBlock = world->getBlock(ceilingHit.blockPos.x,ceilingHit.blockPos.y,ceilingHit.blockPos.z);
                }

                if (groundBlock && camPointer->Position.y < (groundHit.blockPos.y + 2.75)) {
                    //camPointer->Position = camPointer->Position-glm::vec3(glm::ivec3(camPointer->Position)) + glm::vec3(groundHit.blockPos);
                    camPointer->Position.y = groundHit.blockPos.y + 2.8;
                    ground = true;
                    if (camPointer->Velocity.y < 0.0) {
                        camPointer->Velocity.y = 0.0;
                    }
                }
                
                if (ceilingBlock && camPointer->Position.y > (ceilingHit.blockPos.y - 0.15)) {
                    camPointer->Position.y = ceilingHit.blockPos.y - 0.2;
                    ceiling = true;
                    if (camPointer->Velocity.y > 0.0) {
                        camPointer->Velocity.y = 0.0;
                    }
                }
                if (ground && ceiling) {
                    camPointer->Position = previousPosition;
                }

                Block* northBlock = nullptr;    
                Block* southBlock = nullptr;
                Block*  eastBlock = nullptr;
                Block*  westBlock = nullptr;

                // East
                BlockHitResult eastHit = raycast(camPointer->Position, glm::vec3(1.0,0.0,0.0), 1, world, true);
                if (eastHit.hit) {
                    eastBlock = world->getBlock(eastHit.blockPos.x,eastHit.blockPos.y,eastHit.blockPos.z);
                }

                // West
                BlockHitResult westHit = raycast(camPointer->Position, glm::vec3(-1.0,0.0,0.0), 1, world, true);
                if (westHit.hit) {
                    westBlock = world->getBlock(westHit.blockPos.x,westHit.blockPos.y,westHit.blockPos.z);
                }

                // North
                BlockHitResult northHit = raycast(camPointer->Position, glm::vec3(0.0,0.0,1.0), 1, world, true);
                if (northHit.hit) {
                    northBlock = world->getBlock(northHit.blockPos.x,northHit.blockPos.y,northHit.blockPos.z);
                }

                // South
                BlockHitResult southHit = raycast(camPointer->Position, glm::vec3(0.0,0.0,-1.0), 1, world, true);
                if (southHit.hit) {
                    southBlock = world->getBlock(southHit.blockPos.x,southHit.blockPos.y,southHit.blockPos.z);
                }

                // 
                if (eastBlock && camPointer->Position.x > eastHit.blockPos.x - 0.2) {
                    camPointer->Position.x = previousPosition.x;
                    camPointer->Velocity.x = 0.0;
                }

                if (westBlock && camPointer->Position.x < westHit.blockPos.x + 1.2) {
                    camPointer->Position.x = previousPosition.x;
                    camPointer->Velocity.x = 0.0;
                }

                if (northBlock && camPointer->Position.z < northHit.blockPos.z + 1.2) {
                    camPointer->Position.z = previousPosition.z;
                    camPointer->Velocity.z = 0.0;
                }

                if (southBlock && camPointer->Position.z > southHit.blockPos.z - 0.2) {
                    camPointer->Position.z = previousPosition.z;
                    camPointer->Velocity.z = 0.0;
                }
                
                //camPointer->Speed = 4.317;
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
                BlockHitResult hit = raycast(camPointer->Position, camPointer->Orientation, maxDistance, world);

                if (!hit.hit) {
                    debugText += "No block hit.\n";
                } else {
                    debugText +=  "Hit block at: " + std::to_string(hit.blockPos.x) + ", " + std::to_string(hit.blockPos.y) + ", " + std::to_string(hit.blockPos.z) + "\n";
                    Block* b = world->getBlock(hit.blockPos.x,hit.blockPos.y,hit.blockPos.z);
                    if (b) {
                        uint8_t blockType = b->blockType;
                        debugText += "Name: " + getBlockName(blockType) + "\n";
                        debugText += "Id: " + std::to_string(blockType) + "\n";
                        debugText += "MetaData: " + std::to_string(b->metaData) + "\n";
                        debugText += "isTransparent: " + std::to_string(isTransparent(blockType)) + "\n";
                        debugText += "isLightSource: " + std::to_string(isLightSource(blockType)) + "\n";
                        debugText += "isPartialBlock: " + std::to_string(isPartialBlock(blockType)) + "\n";
                        debugText += "isFluid: " + std::to_string(isFluid(blockType)) + "\n";
                    }
                    Block* bn = world->getBlock(hit.blockPos.x+hit.hitNormal.x,hit.blockPos.y+hit.hitNormal.y,hit.blockPos.z+hit.hitNormal.z);
                    if (bn) {
                        debugText += "SkyLight: " + std::to_string(bn->skyLightLevel) + "\n";
                        debugText += "LightLevel: " + std::to_string(bn->lightLevel) + "\n";
                    }
                }
            } else {
                Block* b = world->getBlock(floor(camPointer->Position.x),floor(camPointer->Position.y),floor(camPointer->Position.z));
                if (b) {
                    uint8_t blockType = b->blockType;
                    debugText += "Name: " + getBlockName(blockType) + "\n";
                    debugText += "Id: " + std::to_string(blockType) + "\n";
                    debugText += "MetaData: " + std::to_string(b->metaData) + "\n";
                    debugText += "isTransparent: " + std::to_string(isTransparent(blockType)) + "\n";
                    debugText += "isLightSource: " + std::to_string(isLightSource(blockType)) + "\n";
                    debugText += "isPartialBlock: " + std::to_string(isPartialBlock(blockType)) + "\n";
                    debugText += "isNonSolid: " + std::to_string(isNonSolid(blockType)) + "\n";
                    debugText += "isFluid: " + std::to_string(isFluid(blockType)) + "\n";
                    debugText += "SkyLight: " + std::to_string(b->skyLightLevel) + "\n";
                    debugText += "LightLevel: " + std::to_string(b->lightLevel) + "\n";
                }
            }
            ImGui::SeparatorText("Debug");
            ImGui::Text("%s", debugText.c_str());
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap the back and front buffer
        glfwSwapBuffers(window);
        // Respond to all GLFW events
        glfwPollEvents();
        fpsTime = (glfwGetTime() - prevTime)*1000;
        prevTime = glfwGetTime();
        camPointer->setDelta(fpsTime);
        previousPosition = camPointer->Position;
        if (world) {
            previousRenderedChunks = world->getNumberOfChunks();
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