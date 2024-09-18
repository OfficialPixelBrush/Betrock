#include "config.h"

float skyColor [] = {0.439f, 0.651f, 0.918f, 1.0f};

bool checkIfChunkBoundaryCrossed(glm::vec3 cameraPosition, glm::vec3 previousPosition) {
    int x = int(cameraPosition.x/16);
    int z = int(cameraPosition.z/16);
    int px = int(previousPosition.x/16);
    int pz = int(previousPosition.z/16);
    if (x != px || z != pz) {
        return true;
    }
    return false;
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
    GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight,"Betrock 0.2.0", NULL, NULL);
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

    // Transform the light and cube models
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPosition);
    
    shaderProgram.Activate();
    /*glUniform4f(glGetUniformLocation(shaderProgram.Id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.Id, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform4f(glGetUniformLocation(shaderProgram.Id, "ambient"), skyColor[0], skyColor[1], skyColor[2], skyColor[3]);*/

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Create a camera at 0,0,2
    //Camera camera(windowWidth, windowHeight, glm::vec3(20.392706f, 67.527435f, 90.234566f), glm::vec3(0.604827, -0.490525, 0.627354f));
    Camera camera(windowWidth, windowHeight, glm::vec3(-12, 65.18, 0.88), glm::vec3(0.0, 0.0, 0.9));
    //camera.Position.x -= 0.5;
    //camera.Position.y -= 0.5;
    //camera.Position.z -= 0.5;
    // Draw Clear Color
    glClearColor(skyColor[0],skyColor[1],skyColor[2],skyColor[3]);

    // Makes it so OpenGL shows the triangles in the right order
    // Enables the depth buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Load Blockmodel
    Model blockModel("models/models.obj");

    ChunkBuilder cb(&blockModel);
    worldName = "saves/" + worldName;
    World* world = new World(worldName);

    /*
    Region* r = world.getRegion(0,0);
    for (uint cx = 0; cx < 16; cx++) {
        for (uint cz = 0; cz < 16; cz++) {
            if (c) { 
                Mesh* mesh = cb.build(c,cx,cz);
                loadedChunks.push_back(mesh);
            }
        }
        std::cout << (float(cx)/16.0)*100.0 << "%" << std::endl;
    }*/

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

    double prevTime = glfwGetTime();
    double fpsTime = 0;
    int previousRenderedChunks = 0;
    int maxSkyLight = 15;
    glm::vec3 previousPosition = camera.Position;

    int renderDistance = 3;

    float x = camera.Position.x;
    float z = camera.Position.z;
    world->getChunksInRadius(int(x),int(z),renderDistance);
    std::vector<Mesh*> worldMeshes = cb.build(world,maxSkyLight);

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
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Handle inputs
        if (!io.WantCaptureMouse) {
            camera.Inputs(window);
        }
        camera.updateMatrix(fieldOfView, 0.1f, 300.0f);

        //blockModel.Draw(shaderProgram, camera);
        if (checkIfChunkBoundaryCrossed(camera.Position, previousPosition) && updateWhenMoving) {
            float x = camera.Position.x;
            float z = camera.Position.z;
            world->getChunksInRadius(int(x),int(z),renderDistance);
            worldMeshes = cb.build(world,maxSkyLight);
        }

        for (uint i = 0; i < worldMeshes.size(); i++) {
            worldMeshes[i]->Draw(shaderProgram, camera);
        }

        ImGui::Begin("Options");
        std::string msTime =  "Frame time: " + std::to_string(fpsTime) + "ms/" + std::to_string(1000/fpsTime) + "fps";
        std::string camPos =  "Position: " + std::to_string(camera.Position.x) + ", " + std::to_string(camera.Position.y) + ", " + std::to_string(camera.Position.z);
        std::string chunkPos =  "Chunk: " + std::to_string(int(std::floor(camera.Position.x/16))) + ", " + std::to_string(int(std::floor(camera.Position.z/16)));
        std::string camRot =  "Orientation: " + std::to_string(camera.Orientation.x) + ", " + std::to_string(camera.Orientation.y) + ", " + std::to_string(camera.Orientation.z);
        std::string camSpeed =  "Speed: " + std::to_string(camera.speed);
        std::string currentModels = blockModel.file;
        currentModels += ": \n";
        for (uint i = 0; i < blockModel.meshes.size(); i++) {
            currentModels += blockModel.meshes[i].name + "\n";
        }
        ImGui::Text(msTime.c_str());
        ImGui::Text(camPos.c_str());
        ImGui::Text(chunkPos.c_str());
        ImGui::Text(camRot.c_str());
        ImGui::Text(camSpeed.c_str());
        ImGui::Text(currentModels.c_str());
        ImGui::ColorEdit4("Sky Color", skyColor);
        ImGui::Checkbox("Vsync", &vsync);
        ImGui::Checkbox("Backface Culling", &cullFace);
        ImGui::Checkbox("Polygon", &polygon);
        ImGui::Checkbox("Update when Moving", &updateWhenMoving);
        ImGui::SliderInt("Skylight",&maxSkyLight, 0, 15);
        ImGui::SliderInt("Render Distance",&renderDistance, 2, 16);
        if (ImGui::Button("Update Chunks")) {
            float x = camera.Position.x;
            float z = camera.Position.z;
            world->getChunksInRadius(int(x),int(z),renderDistance);
            worldMeshes = cb.build(world,maxSkyLight);
            for (uint i = 0; i < worldMeshes.size(); i++) {
                worldMeshes[i]->Draw(shaderProgram, camera);
            }
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