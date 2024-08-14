#include "config.h"
#define textureConst 0.0625
#define blockX 2
#define blockY 15


// Vertices coordinates
/*
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};*/

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

struct faceElement {
    GLuint vertex;
    //GLuint texture;
    GLuint normal;
};

// Ported from my old MS-DOS 3D Attempt
Mesh* tempObject(const char* filename, std::vector <Texture> tex) {
    FILE * fp;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<struct faceElement> indices;
    char str[256];
    fp = fopen(filename, "r");
    int trash = 0;

	if (fp != NULL) {
		printf("Found %s\n", filename);
		while (fgets(str, sizeof(str), fp) != NULL){
			if(str[0] == 'v' && str[1] == ' ') {
				/* Load verts */
                float x,y,z = 0;
				sscanf(str, "v %f %f %f", &x,&y,&z);
                vertices.push_back(glm::vec3(x, y, z));
			} else if(str[0] == 'v' && str[1] == 'n' && str[2] == ' ') {
				/* Load verts */
                float x,y,z = 0;
				sscanf(str, "vn %f %f %f", &x,&y,&z);
                normals.push_back(glm::vec3(x,y,z));
			} else if(str[0] == 'f' && str[1] == ' ') {
				/* Load faces */
                GLuint vx,vy,vz = 0;
                GLuint nx,ny,nz = 0;
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vx,&trash,&nx, &vy,&trash,&ny, &vz,&trash,&nz);
                struct faceElement fx;
                fx.vertex = vx-1;
                fx.normal = nx-1;
                struct faceElement fy;
                fy.vertex = vy-1;
                fy.normal = ny-1;
                struct faceElement fz;
                fz.vertex = vz-1;
                fz.normal = nz-1;
                indices.push_back(fx);
                indices.push_back(fy);
                indices.push_back(fz);
			}
		}
		fclose(fp);
	} else {
		printf("Failed to find %s!\n", filename);
		return nullptr;
	}
    std::vector<Vertex> vert;
    for (uint i = 0; i < vertices.size(); i++) {
        GLuint normalIndex = 0;
        // Check which normal belongs to which vertex
        for (uint j = 0; j < indices.size(); j++) {
            struct faceElement fe = indices[j];
            if (fe.vertex == i) {
                normalIndex = fe.normal;
                break;
            }
        }

        vert.push_back(Vertex{vertices[i], normals[normalIndex], normals[normalIndex], glm::vec2(0.0f, 0.0f)});
    }

    std::vector<GLuint> vertexIndices(indices.size());
    for (size_t i = 0; i < indices.size(); i++) {
        vertexIndices[i] = indices[i].vertex;
    }

    return new Mesh(vert,vertexIndices,tex);
}

glm::vec4 skyColor(0.439f, 0.651f, 0.918f, 1.0f);
//glm::vec4 skyColor(0.1, 0.1, 0.1, 1.0f);

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

    // Texture
    // Import texture via file
    Texture textures[] {
        Texture("../textures/terrain.png" , "diffuse" , 0, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("../textures/specular.png", "specular", 1, GL_RED , GL_UNSIGNED_BYTE)
    };

    // Creates Shader object using shaders default.vert and .frag
    //Shader shaderProgram("../src/shader/default.vert", "../src/shader/default.frag");
    Shader shaderProgram("../src/shader/default.vert", "../src/shader/normal.frag");
    //std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
    //std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
    std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
    //Mesh floor(verts, ind, tex);

    Mesh floor = *tempObject("../src/models/cube.obj", tex);


    // Create the light and send it to the GPU
    Shader lightShader("../src/shader/light.vert", "../src/shader/light.frag");
    std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
    std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
    Mesh light(lightVerts, lightInd, tex);

    // Transform the light and cube models
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPosition);
    
    glm::vec3 objectPosition = glm::vec3(0.0, 0.0, 0.0);
    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel = glm::translate(objectModel, objectPosition);

    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.Id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.Id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    shaderProgram.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.Id, "model"), 1, GL_FALSE, glm::value_ptr(objectModel));
    glUniform4f(glGetUniformLocation(shaderProgram.Id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.Id, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform4f(glGetUniformLocation(shaderProgram.Id, "ambient"), skyColor.x, skyColor.y, skyColor.z, skyColor.w);

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

        floor.Draw(shaderProgram, camera);
        light.Draw(lightShader, camera);
        
        // Swap the back and front buffer
        glfwSwapBuffers(window);
        // Respond to all GLFW events
        glfwPollEvents();
        //std::cout << (glfwGetTime() - prevTime)*1000 << " ms" << std::endl;
        prevTime = glfwGetTime();
    }

    // Clean-up
    //shaderProgram.Delete();
    //lightShader.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}