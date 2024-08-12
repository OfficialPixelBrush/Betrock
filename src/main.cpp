#include "config.h"
#define textureConst 0.0625
#define blockX 2
#define blockY 15

// Vertex Buffer
GLfloat vertices[] = { // Pos  / Color       / UV
    -0.5, -0.5, 0.0,            1.0, 0.0, 0.0, blockX*textureConst, blockY*textureConst,
    -0.5,  0.5, 0.0,            0.0, 1.0, 0.0, blockX*textureConst, (blockY+1)*textureConst,
     0.5,  0.5, 0.0,            0.0, 0.0, 1.0, (blockX+1)*textureConst, (blockY+1)*textureConst,
     0.5, -0.5, 0.0,            1.0, 1.0, 1.0, (blockX+1)*textureConst, blockY*textureConst
};


GLuint indices[] = {
    0, 2, 1,
    0, 3, 2
};

// Targeting OpenGL 3.3
int main() {
    region r = region(0,0);
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
        // Respond to all GLFW events
        glfwPollEvents();
        // Draw
        glClearColor(0.439f, 0.651f, 0.918f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderProgram.Activate();
        // Apply scaling uniform
        glUniform1f(uniId,0.8f);
        terrain.Bind();
        vao1.Bind();
        // Draw the Triangle
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
        // Swap the back and front buffer
        glfwSwapBuffers(window);
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