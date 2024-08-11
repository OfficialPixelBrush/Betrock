#include "config.h"

// Load file as char*
char* loadSource(string path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        return NULL;
    }
    // Get file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    // Allocate memory for the char array
    char* buffer = new char[int(fileSize) + 1];
    // +1 for null terminator
    // Read the file into the buffer
    file.read(buffer, fileSize);
    buffer[fileSize] = '\0'; // Null terminate the string
    cout << "Read " << path << "\n";
    return buffer;
}

char* vertexShaderSource = loadSource("../src/shaders/vertex.h");
char* fragmentShaderSource = loadSource("../src/shaders/fragment.h");

// Targeting OpenGL 3.3
int main() {
    int windowWidth = 1280;
    int windowHeight = 720;
    glfwInit();

    // Specify OpenGL Version and Feature-set
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Vertex Buffer
    GLfloat vertecies[] = {
        -0.5f   , -0.5f * float(sqrt(3)) / 3    , 0.0f,
         0.5f   , -0.5f * float(sqrt(3)) / 3    , 0.0f,
         0.0f   ,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,
        -0.5 / 2,  0.5f * float(sqrt(3)) / 6    , 0.0f,
         0.5 / 2,  0.5f * float(sqrt(3)) / 6    , 0.0f,
         0.0 / 2, -0.5f * float(sqrt(3)) / 3    , 0.0f,
    };

    GLuint indices[] = {
        0,3,5,
        3,2,4,
        5,4,1
    };

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

    // Create Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Create Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create Shader Program and Link
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete Vertex and Fragment Shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VAO = Vertex Array Object
    // Help to switch between multiple VBOs
    // VBO = Vertex Buffer Object
    GLuint VAO, VBO, EBO;

    // Gnerate the VAO and VBO with only 1 object each
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // EBO Used to hold indices
    glGenBuffers(1, &EBO);

    // Set VAO as current target
    glBindVertexArray(VAO);

    // Set the VBO as our current target object
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Load the vertecies into the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertecies), vertecies, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Configure the Vertex Attribute so OpenGL knows how to read the VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    // Enable the Vertex Attribute so that OpenGL knows to use it
    glEnableVertexAttribArray(0);

    // Unbind VAO and VBO so they cannot be modified any further.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Draw Clear Color
    glClearColor(0.439f, 0.651f, 0.918f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    region r = region(0,0);

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        // Respond to all GLFW events
        glfwPollEvents();
        // Draw
        glClearColor(0.439f, 0.651f, 0.918f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // Draw the Triangle
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
        // Swap the back and front buffer
        glfwSwapBuffers(window);
    }

    // Clean-up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}