#include "config.h"

float skyColor [] = {0.439f, 0.651f, 0.918f, 1.0f};

// Targeting OpenGL 3.3
int main() {
    float fieldOfView = 70.0f;
    int windowWidth = 1280;
    int windowHeight = 720;
    glfwInit();

    // Specify OpenGL Version and Feature-set
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight,"Betrock 0.1.0", NULL, NULL);
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

    // Create a camera at 0,0,2
    Camera camera(windowWidth, windowHeight, glm::vec3(20.392706f, 67.527435f, 90.234566f), glm::vec3(0.604827, -0.490525, 0.627354f));
    // Draw Clear Color
    glClearColor(skyColor[0],skyColor[1],skyColor[2],skyColor[3]);

    // Makes it so OpenGL shows the triangles in the right order
    // Enables the depth buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Load Blockmodel
    Model blockModel("models/models.obj");

    ChunkBuilder cb(&blockModel);
    std::vector<Mesh*> loadedChunks;
    World world("saves/glacier");
    Region* r = world.getRegion(0,0);
    for (uint cx = 0; cx < 16; cx++) {
        for (uint cz = 0; cz < 16; cz++) {
            Chunk* c = r->getChunk(cx,cz);
            if (c) { 
                Mesh* mesh = cb.build(c,cx,cz);
                loadedChunks.push_back(mesh);
            }
        }
        std::cout << (float(cx)/16.0)*100.0 << "%" << std::endl;
    }

    // ImGui Addition
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool vsync = true;
    bool cullFace = true;

    double prevTime = glfwGetTime();
    double fpsTime = 0;

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        // Update Options
        if (vsync) {
            glfwSwapInterval( 1 );
        } else {
            glfwSwapInterval( 0 );
        }
        if (cullFace) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
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

        for (uint i = 0; i < loadedChunks.size(); i++) {
            loadedChunks[i]->Draw(shaderProgram, camera);
        }

        ImGui::Begin("Options");
        std::string msTime =  "Frame time: " + std::to_string(fpsTime) + "ms/" + std::to_string(1000/fpsTime) + "fps";
        std::string camPos =  "Position: " + std::to_string(camera.Position.x) + ", " + std::to_string(camera.Position.y) + ", " + std::to_string(camera.Position.z);
        std::string camRot =  "Orientation: " + std::to_string(camera.Orientation.x) + ", " + std::to_string(camera.Orientation.y) + ", " + std::to_string(camera.Orientation.z);
        std::string camSpeed =  "Speed: " + std::to_string(camera.speed);
        std::string currentModels = blockModel.file;
        currentModels += ": \n";
        for (uint i = 0; i < blockModel.meshes.size(); i++) {
            currentModels += blockModel.meshes[i].name + "\n";
        }
        ImGui::Text(msTime.c_str());
        ImGui::Text(camPos.c_str());
        ImGui::Text(camRot.c_str());
        ImGui::Text(camSpeed.c_str());
        ImGui::Text(currentModels.c_str());
        ImGui::ColorEdit4("Sky Color", skyColor);
        ImGui::Checkbox("Vsync", &vsync);
        ImGui::Checkbox("Backface Culling", &cullFace);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap the back and front buffer
        glfwSwapBuffers(window);
        // Respond to all GLFW events
        glfwPollEvents();
        fpsTime = (glfwGetTime() - prevTime)*1000;
        prevTime = glfwGetTime();
    }

    // Clean-up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}