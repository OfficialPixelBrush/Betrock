#include "config.h"
#define textureConst 0.0625
#define blockX 2
#define blockY 15

// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     / COLORS       /    TexCoord   /        NORMALS       //
	-0.5f, 0.0f,  0.5f,     1, 1, 1, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	-0.5f, 0.0f, -0.5f,     1, 1, 1,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f, -0.5f,     1, 1, 1,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f,  0.5f,     1, 1, 1,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

	-0.5f, 0.0f,  0.5f,     1, 1, 1, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	-0.5f, 0.0f, -0.5f,     1, 1, 1,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	 0.0f, 0.8f,  0.0f,     1, 1, 1,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side

	-0.5f, 0.0f, -0.5f,     1, 1, 1,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.5f, 0.0f, -0.5f,     1, 1, 1,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.0f, 0.8f,  0.0f,     1, 1, 1,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

	 0.5f, 0.0f, -0.5f,     1, 1, 1,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.5f, 0.0f,  0.5f,     1, 1, 1,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.0f, 0.8f,  0.0f,     1, 1, 1,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side

	 0.5f, 0.0f,  0.5f,     1, 1, 1,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	-0.5f, 0.0f,  0.5f,     1, 1, 1, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	 0.0f, 0.8f,  0.0f,     1, 1, 1,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2, // Bottom side
	0, 2, 3, // Bottom side
	4, 6, 5, // Left side
	7, 9, 8, // Non-facing side
	10, 12, 11, // Right side
	13, 15, 14 // Facing side
};


// Example Pyramid
GLfloat lightVertices[] = { // Pos  / Color       / UV
    -.2, 0,  .2,
    -.2, 0, -.2,
     .2, 0, -.2,
     .2, 0,  .2,
      0, .3,  0
};


GLuint lightIndices[] = {
    0,1,2,
    0,2,3,
    0,1,4,
    1,2,4,
    2,3,4,
    3,0,4
};

//glm::vec4 skyColor(0.439f, 0.651f, 0.918f, 1.0f);
glm::vec4 skyColor(0.2, 0.2, 0.2, 1.0f);

// Targeting OpenGL 3.3
int main() {
    //region r = region(0,0);
    World world("world");
    Region* r = world.getRegion(0,0);
    Block* blocks = r->getChunk(0,0)->getData();
    for (uint i = 0; i < 128; i++) {
        std::cout << std::to_string(blocks[i].getBlock()) << ",";
    }
    std::cout << std::endl;


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
    vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 11*sizeof(float), (void*)0);
    vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 11*sizeof(float), (void*)(3*sizeof(float)));
    vao1.LinkAttrib(vbo1, 2, 2, GL_FLOAT, 11*sizeof(float), (void*)(6*sizeof(float)));
    vao1.LinkAttrib(vbo1, 3, 3, GL_FLOAT, 11*sizeof(float), (void*)(8*sizeof(float)));

    // Clean up our binds for new stuff
    vao1.Unbind();
    vbo1.Unbind();
    ebo1.Unbind();

    // Create the light and send it to the GPU
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    Shader lightShader("../src/shader/light.vert", "../src/shader/light.frag");
    VAO lightVAO;
    lightVAO.Bind();

    VBO lightVBO(lightVertices, sizeof(lightVertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));

    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3*sizeof(float), (void*)0);
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    // Transform the light and cube models
    glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPosition);
    
    glm::vec3 cubePosition = glm::vec3(0.0, 0.0, 0.0);
    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::translate(cubeModel, cubePosition);

    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.Id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.Id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    shaderProgram.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.Id, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
    glUniform4f(glGetUniformLocation(shaderProgram.Id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.Id, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);


    // Texture
    // Import texture via file
    Texture terrain("../textures/terrain.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    // Yeet it over to the GPU
    terrain.textureUnit(shaderProgram, "texture0", 0);

    glEnable(GL_DEPTH_TEST);

    // Create a camera at 0,0,2
    Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    // Draw Clear Color
    glClearColor(skyColor.x, skyColor.y, skyColor.z, skyColor.w);

    // Makes it so OpenGL shows the triangles in the right order
    // Enables the depth buffer
    glClear(GL_COLOR_BUFFER_BIT);

    double prevTime = glfwGetTime();

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        // Draw
        glClearColor(skyColor.x, skyColor.y, skyColor.z, skyColor.w);
        // Clear the Back and Depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Handle inputs
        camera.Inputs(window);
        camera.updateMatrix(fieldOfView, 0.1f, 100.0f);

        // Tell OpenGL to use our shader
        shaderProgram.Activate();
        // Send camera position to shader
        glUniform3f(glGetUniformLocation(shaderProgram.Id, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
        camera.Matrix(shaderProgram, "cameraMatrix");

        // Binds the texture so it appears when rendering
        terrain.Bind();
        // Binds the VAO so OpenGL knows to use it
        vao1.Bind();
        // Draw the Triangle
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

        lightShader.Activate();
        camera.Matrix(lightShader, "cameraMatrix");
        lightVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices)/sizeof(int), GL_UNSIGNED_INT, 0);

        // Swap the back and front buffer
        glfwSwapBuffers(window);
        // Respond to all GLFW events
        glfwPollEvents();
        //std::cout << (glfwGetTime() - prevTime)*1000 << " ms" << std::endl;
        prevTime = glfwGetTime();
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