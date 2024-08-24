#include "config.h"

float skyColor [] = {0.439f, 0.651f, 0.918f, 1.0f};

// Targeting OpenGL 3.3
int main() {
    //region r = region(0,0);
    //World world("world");
    //Region* r = world.getRegion(0,0);

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
    Shader shaderProgram("../src/shader/default.vsh", "../src/shader/default.fsh");

    // Transform the light and cube models
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPosition);
    
    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.Id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.Id, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform4f(glGetUniformLocation(shaderProgram.Id, "ambient"), skyColor[0], skyColor[1], skyColor[2], skyColor[3]);

    glEnable(GL_DEPTH_TEST);

    // Create a camera at 0,0,2
    Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    // Draw Clear Color
    glClearColor(skyColor[0],skyColor[1],skyColor[2],skyColor[3]);

    // Makes it so OpenGL shows the triangles in the right order
    // Enables the depth buffer
    glClear(GL_COLOR_BUFFER_BIT);

    double prevTime = glfwGetTime();
    double fpsTime = 0;

    Model model("models/sword/scene.gltf");
    /*
    int chunkPos [2] = {0,0};
    Chunk* c = r->getChunk(chunkPos[0],chunkPos[1]);
    ChunkBuilder cb;
    Mesh* chunk = cb.build(blockModel,c, tex);
    */

    // ImGui Addition
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
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
        camera.updateMatrix(fieldOfView, 0.1f, 100.0f);

        model.Draw(shaderProgram, camera);

        //chunk->Draw(shaderProgram, camera);

        ImGui::Begin("Options");
        std::string msTime =  "Frame time: " + std::to_string(fpsTime) + "ms";
        std::string camPos =  "Position: " + std::to_string(camera.Position.x) + ", " + std::to_string(camera.Position.y) + ", " + std::to_string(camera.Position.z);
        ImGui::Text(msTime.c_str());
        ImGui::Text(camPos.c_str());
        ImGui::ColorEdit4("Sky Color", skyColor);
        /*ImGui::SliderInt2("Chunk", chunkPos, 0, 8);
        if (ImGui::Button("Load"))
        {
            c = r->getChunk(chunkPos[0],chunkPos[1]);
            chunk = cb.build(blockModel,c, tex);
        }*/
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