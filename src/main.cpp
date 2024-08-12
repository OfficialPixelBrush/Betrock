#include "config.h"
#define textureConst 0.0625
#define blockX 2
#define blockY 15

// Vertex Buffer
GLfloat vertices[] = { // Pos  / Color       / UV
    // Front
    -0.5, -0.5, 0.0,            1.0, 1.0, 1.0, blockX*textureConst, blockY*textureConst, // Bottom Left
    -0.5,  0.5, 0.0,            1.0, 1.0, 1.0, blockX*textureConst, (blockY+1)*textureConst, // Top Left
     0.5,  0.5, 0.0,            1.0, 1.0, 1.0, (blockX+1)*textureConst, (blockY+1)*textureConst, // Top Right
     0.5, -0.5, 0.0,            1.0, 1.0, 1.0, (blockX+1)*textureConst, blockY*textureConst, // Bottom Right
     // Back
    -0.5, -0.5, -1.0,            1.0, 1.0, 1.0, blockX*textureConst, blockY*textureConst, // Bottom Left
    -0.5,  0.5, -1.0,            1.0, 1.0, 1.0, blockX*textureConst, (blockY+1)*textureConst, // Top Left
     0.5,  0.5, -1.0,            1.0, 1.0, 1.0, (blockX+1)*textureConst, (blockY+1)*textureConst, // Top Right
     0.5, -0.5, -1.0,            1.0, 1.0, 1.0, (blockX+1)*textureConst, blockY*textureConst // Bottom Right
};


GLuint indices[] = {
    0,1,2, // Front
    0,2,3,
    1,5,6, // Top
    1,6,2, 
    0,1,5, // Left
    0,5,4,
    4,5,6, // Back
    4,6,7,
    0,4,7, // Bottom
    0,3,7,
    2,3,6, // Right
    3,6,7
};

// Targeting OpenGL 3.3
int main() {
    region r = region(0,0);
    float fieldOfView = 45.0f;
    int windowWidth = 1280;
    int windowHeight = 720;
    glfwInit();

    // Specify OpenGL Version and Feature-set
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight,"Betrock", NULL, NULL);
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
    glViewport(0,0,windowWidth,windowHeight);

    // Creates Shader object using shaders default.vert and .frag
    Shader shaderProgram("../src/shader/default.vert", "../src/shader/default.frag");

    // Create Vertex Array Object and Bind it
    VAO vao1;
    vao1.Bind();

    // Generates Vertex Buffer Object and links it to vertices
    VBO vbo1(vertices, sizeof(vertices));
    // Same with the Element Buffer Object, just for indices
    EBO ebo1(indices, sizeof(indices));

    // Link that shit
    vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 8*sizeof(float), (void*)0);
    vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 8*sizeof(float), (void*)(3*sizeof(float)));
    vao1.LinkAttrib(vbo1, 2, 2, GL_FLOAT, 8*sizeof(float), (void*)(6*sizeof(float)));

    // Clean up our binds for new stuff
    vao1.Unbind();
    vbo1.Unbind();
    ebo1.Unbind();

    GLuint uniId = glGetUniformLocation(shaderProgram.Id, "scale");

    // Texture
    // Import texture via file
    Texture terrain("../textures/terrain.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    // Yeet it over to the GPU
    terrain.textureUnit(shaderProgram, "texture0", 0);


    // Draw Clear Color
    glClearColor(0.439f, 0.651f, 0.918f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        // Draw
        glClearColor(0.439f, 0.651f, 0.918f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Tell OpenGL to use our shader
        shaderProgram.Activate();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
        // Set fov, size, near and far clip plane
        projection = glm::perspective(glm::radians(fieldOfView), (float)(windowWidth/windowHeight), 0.1f, 100.0f);

        int modelLocation = glGetUniformLocation(shaderProgram.Id, "model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        int viewLocation = glGetUniformLocation(shaderProgram.Id, "view");
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLocation = glGetUniformLocation(shaderProgram.Id, "projection");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        // Apply scaling uniform
        glUniform1f(uniId,0.8f);
        // Binds the texture so it appears when rendering
        terrain.Bind();
        // Binds the VAO so OpenGL knows to use it
        vao1.Bind();
        // Draw the Triangle
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
        // Swap the back and front buffer
        glfwSwapBuffers(window);
        // Respond to all GLFW events
        glfwPollEvents();
    }

    // Clean-up
    vao1.Delete();
    vbo1.Delete();
    ebo1.Delete();
    terrain.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}