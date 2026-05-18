/**
 * src/platform/nx_main.cpp
 *
 * Nintendo Switch entry point.
 * Replaces the desktop src/main.cpp entirely for the NX build.
 *
 * Differences from the PC build:
 *  - Window / context created with libnx EGL helpers, not GLFW.
 *  - Glad is loaded against OpenGL ES 3.2 (gladLoadGLES2Loader).
 *  - Input is handled via libnx padStateUpdate / hidAnalogStickVal.
 *  - ImGui uses imgui_impl_opengl3 (IMGUI_IMPL_OPENGL_ES3) + our own
 *    nx_imgui_nx.cpp backend (no glfw backend).
 *  - Asset path prefix is "romfs:/" (see global.h basePath() usage).
 *  - No fullscreen toggle (Switch is always fullscreen at 1280×720).
 *  - Screenshots use stb_image_write to sdmc:/ instead of the local dir.
 */

#include "global.h"
#include "helper.h"
#include "compat.h"
#include "version.h"
#include <fstream>
#include <math.h>
#include <thread>
#include <mutex>

// libnx
#include <switch.h>

// EGL / OpenGL ES
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <glad/glad.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ImGui (GLES3 backend selected via IMGUI_IMPL_OPENGL_ES3 define in CMake)
#include "include/imgui/imgui.h"
#include "include/imgui/backends/imgui_impl_opengl3.h"
#include "platform/nx_imgui_nx.h"   // our Switch-specific ImGui input backend

#include "include/stb/stb_image_write.h"

#include "render/mesh.h"
#include "render/sky.h"
#include "render/camera_nx.h"    // Switch camera (no GLFWwindow* param)
#include "model/chunkBuilder.h"
#include "model/model.h"

#include "world/world.h"
#include "world/block.h"
#include "world/blockProperties.h"

// ---------------------------------------------------------------------------
// Switch is always 1280×720 (handheld) or 1920×1080 (docked); we target 720p.
// ---------------------------------------------------------------------------
static constexpr int SCREEN_W = 1280;
static constexpr int SCREEN_H = 720;

// ---------------------------------------------------------------------------
// EGL state
// ---------------------------------------------------------------------------
static EGLDisplay s_display = EGL_NO_DISPLAY;
static EGLContext s_context  = EGL_NO_CONTEXT;
static EGLSurface s_surface  = EGL_NO_SURFACE;

static bool initEGL()
{
    s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (s_display == EGL_NO_DISPLAY) {
        std::cerr << "Could not get EGL display\n";
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(s_display, &major, &minor)) {
        std::cerr << "eglInitialize failed\n";
        return false;
    }

    // Request OpenGL ES 3.x
    static const EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_DEPTH_SIZE,      24,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfig;
    if (!eglChooseConfig(s_display, configAttribs, &config, 1, &numConfig) || numConfig == 0) {
        std::cerr << "eglChooseConfig failed\n";
        return false;
    }

    eglBindAPI(EGL_OPENGL_ES_API);

    static const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, contextAttribs);
    if (s_context == EGL_NO_CONTEXT) {
        std::cerr << "eglCreateContext failed\n";
        return false;
    }

    // libnx exposes nwindowGetDefault() for the native window handle
    s_surface = eglCreateWindowSurface(s_display, config,
                                       (EGLNativeWindowType)nwindowGetDefault(),
                                       nullptr);
    if (s_surface == EGL_NO_SURFACE) {
        std::cerr << "eglCreateWindowSurface failed\n";
        return false;
    }

    eglMakeCurrent(s_display, s_surface, s_surface, s_context);
    return true;
}

static void deinitEGL()
{
    if (s_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (s_context != EGL_NO_CONTEXT) eglDestroyContext(s_display, s_context);
        if (s_surface != EGL_NO_SURFACE) eglDestroySurface(s_display, s_surface);
        eglTerminate(s_display);
    }
}

// ---------------------------------------------------------------------------
// Shared chunk-building state (identical logic to desktop main.cpp)
// ---------------------------------------------------------------------------
struct BlockHitResult {
    bool hit;
    glm::ivec3 blockPos;
    glm::vec3  hitPos;
    glm::vec3  hitNormal;
};

std::vector<ChunkMesh*> chunkMeshes;
std::vector<DummyMesh>  meshBuildQueue;
std::mutex chunkMeshesMutex;
std::mutex meshBuildQueueMutex;
std::mutex chunkRadiusMutex;
Camera* camPointer = nullptr;

static bool checkIfChunkBoundaryCrossed(glm::vec3 cam, glm::vec3 prev)
{
    int x  = int(floor(cam.x  / CHUNK_WIDTH) + 0.5f);
    int z  = int(floor(cam.z  / CHUNK_WIDTH) + 0.5f);
    int px = int(floor(prev.x / CHUNK_WIDTH) + 0.5f);
    int pz = int(floor(prev.z / CHUNK_WIDTH) + 0.5f);
    return (px != x || pz != z);
}

static BlockHitResult raycast(glm::vec3 origin, glm::vec3 direction,
                               float maxDist, World* world,
                               bool checkSolidity = false)
{
    if (!world) return {false, {}, {}, {}};

    glm::ivec3 cur  = glm::floor(origin);
    glm::vec3  dd   = glm::abs(glm::vec3(1.0f) / direction);
    glm::ivec3 step;
    glm::vec3  side;

    for (int i = 0; i < 3; i++) {
        if (direction[i] < 0) { step[i] = -1; side[i] = (origin[i] - cur[i]) * dd[i]; }
        else                  { step[i] =  1; side[i] = (cur[i] + 1.0f - origin[i]) * dd[i]; }
    }

    float rayLen = 0.0f;
    glm::vec3 hitNorm(0.0f);

    while (rayLen < maxDist) {
        Block* b = world->getBlock(cur.x, cur.y, cur.z);
        if (b && b->blockType != AIR) {
            if (!checkSolidity || !isNonSolid(b->blockType)) {
                glm::vec3 hp = origin + direction * rayLen;
                return {true, cur, hp, hitNorm};
            }
        }
        if (side.x < side.y && side.x < side.z) {
            cur.x += step.x; rayLen = side.x; side.x += dd.x;
            hitNorm = glm::vec3(-step.x, 0.0f, 0.0f);
        } else if (side.y < side.z) {
            cur.y += step.y; rayLen = side.y; side.y += dd.y;
            hitNorm = glm::vec3(0.0f, -step.y, 0.0f);
        } else {
            cur.z += step.z; rayLen = side.z; side.z += dd.z;
            hitNorm = glm::vec3(0.0f, 0.0f, -step.z);
        }
    }
    return {false, {}, {}, {}};
}

void buildChunks(Model* blockModel, World* world, bool& smoothLighting, int& skyLight, std::vector<Chunk*>& toBeUpdated) {
    ChunkBuilder cb(blockModel, world);
    bool building = false;

    std::cout << "BuildChunk Thread lives!" << std::endl;

    while (true) {
        // Check if there are chunks to update
        Chunk* c = nullptr;
        {
            std::unique_lock<std::mutex> lock(chunkRadiusMutex);
            if (!toBeUpdated.empty()) {
                c = toBeUpdated.front();
                toBeUpdated.erase(toBeUpdated.begin());
            }
        }

        if (c) {
            building = true;

            // CPU-side mesh generation only
            DummyMesh cpuMesh = cb.buildChunk(c, smoothLighting, skyLight);

            // Push to queue for main thread to upload
            {
                std::unique_lock<std::mutex> lock(meshBuildQueueMutex);
                meshBuildQueue.push_back(std::move(cpuMesh));
            }
        } else {
            building = false;
            // Only sleep when there's nothing to do, to avoid busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}


// ---------------------------------------------------------------------------
// Screenshot helper  (writes to sdmc:/ on Switch)
// ---------------------------------------------------------------------------
static void takeScreenshot()
{
    int w = SCREEN_W, h = SCREEN_H;
    std::vector<uint8_t> pixels(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Flip vertically
    for (int y = 0; y < h / 2; y++) {
        std::swap_ranges(pixels.begin() + y * w * 3,
                         pixels.begin() + (y + 1) * w * 3,
                         pixels.begin() + (h - 1 - y) * w * 3);
    }

    // Write to SD card
    static int screenshotNum = 0;
    std::string path = "sdmc:/BetrockViewer_" + std::to_string(screenshotNum++) + ".png";
    stbi_write_png(path.c_str(), w, h, 3, pixels.data(), w * 3);
    std::cout << "Screenshot saved: " << path << "\n";
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int /*argc*/, char** /*argv*/)
{
    // libnx init
    consoleInit(nullptr);   // optional: debug console on second screen
    romfsInit();
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    PadState pad;
    padInitializeDefault(&pad);

    // Set romfs as the base data path
    BetrockViewer::basePath() = "romfs:";
    std::cout << "Base data directory: romfs:\n";

    // EGL + GLES
    if (!initEGL()) {
        std::cerr << "EGL init failed\n";
        deinitEGL();
        romfsExit();
        return -1;
    }

    // Load OpenGL ES function pointers via glad
    /*
    if (!gladLoadGLLoader((GLADloadproc)eglGetProcAddress)) {
        std::cerr << "Failed to load GLES2 via glad\n";
        deinitEGL();
        romfsExit();
        return -1;
    }
    */

    glViewport(0, 0, SCREEN_W, SCREEN_H);
    std::cout << "GL Version: " << glGetString(GL_VERSION) << "\n";

    // Shader paths via romfs
    auto shaderPath = [](const char* name) -> std::string {
        return std::string("romfs:/shaders/") + name;
    };

    Shader blockShader  (shaderPath("default.vsh").c_str(), shaderPath("minecraft.fsh").c_str());
    Shader normalShader (shaderPath("default.vsh").c_str(), shaderPath("normal.fsh").c_str());
    Shader defaultShader(shaderPath("default.vsh").c_str(), shaderPath("default.fsh").c_str());
    Shader skyShader    (shaderPath("sky.vsh").c_str(),     shaderPath("sky.fsh").c_str());
    std::cout << "Shaders loaded.\n";

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)SCREEN_W, (float)SCREEN_H);

    ImGui::StyleColorsDark();
    ImGui_ImplNX_Init();               // our Switch ImGui backend
    ImGui_ImplOpenGL3_Init("#version 300 es");

    // World / camera defaults (same as desktop)
    static char worldName[256] = "publicbeta";
    camPointer = new Camera(SCREEN_W, SCREEN_H,
                             glm::vec3(47.00f, 67.62f, 225.59f),
                             glm::vec3(0.46f, -0.09f, 0.76f));

    World* world = nullptr;
    bool worldLoaded = false;
    bool smoothLighting = true;
    bool showUI = true;

    Model* blockModel = new Model(("romfs:/models/models.obj"));
    Model* skyModel   = new Model(("romfs:/models/sky.obj"));
    Sky sky((Mesh*)skyModel);

    std::vector<Chunk*> toBeUpdated;
    int maxSkyLight = 15;
    std::thread chunkBuildingThread(buildChunks, blockModel, world,
                                     std::ref(smoothLighting),
                                     std::ref(maxSkyLight),
                                     std::ref(toBeUpdated));
    chunkBuildingThread.detach();

    float fieldOfView = 70.0f;
    glm::vec3 previousPosition = camPointer->Position;
    double prevTime = armGetSystemTick() / 19200000.0;

    // ── Main loop ──────────────────────────────────────────────────────────────
    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        // + button quits
        if (kDown & HidNpadButton_Plus) break;

        double now = armGetSystemTick() / 19200000.0;
        double fpsTime = (now - prevTime) * 1000.0;
        prevTime = now;
        camPointer->SetDelta(fpsTime);

        // Camera movement via left stick + right stick
        camPointer->ProcessInput(pad);

        // Screenshot via ZL+ZR
        if ((padGetButtons(&pad) & (HidNpadButton_ZL | HidNpadButton_ZR))
              == (HidNpadButton_ZL | HidNpadButton_ZR))
        {
            takeScreenshot();
        }

        // Clear
        float skyMul = maxSkyLight / 15.0f;
        glClearColor(sky.skyColor[0]*skyMul, sky.skyColor[1]*skyMul,
                     sky.skyColor[2]*skyMul, sky.skyColor[3]*skyMul);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camPointer->UpdateMatrix(fieldOfView, 0.1f,
            world ? std::max(128.0f, (float)32*16) : 200.0f);
        camPointer->UploadMatrix(blockShader,  "cameraMatrix");
        camPointer->UploadMatrix(normalShader, "cameraMatrix");
        camPointer->UploadMatrix(defaultShader,"cameraMatrix");
        camPointer->UploadMatrix(skyShader,    "cameraMatrix");

        // Draw sky
        sky.Draw(skyShader, *camPointer);

        // Draw world chunks (same as desktop)
        {
            std::lock_guard<std::mutex> lock(chunkMeshesMutex);
            for (ChunkMesh* cm : chunkMeshes) {
                if (cm) cm->Draw(blockShader, *camPointer);
            }
        }

        // ImGui UI
        ImGui_ImplNX_NewFrame(pad);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        if (showUI) {
            ImGui::Begin("BetrockViewer");
            ImGui::Text("FPS: %.1f", 1000.0 / std::max(fpsTime, 0.001));
            ImGui::Text("Pos: %.1f %.1f %.1f",
                        camPointer->Position.x,
                        camPointer->Position.y,
                        camPointer->Position.z);
            if (ImGui::InputText("World", worldName, sizeof(worldName))) {}
            if (ImGui::Button("Load World")) {
                // Load world from romfs saves directory
                std::string wPath = std::string("romfs:/saves/") + worldName;
                delete world;
                world = new World(wPath.c_str());
                worldLoaded = true;
            }
            ImGui::SliderInt("Sky Light", &maxSkyLight, 0, 15);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        eglSwapBuffers(s_display, s_surface);

        previousPosition = camPointer->Position;
    }

    // ── Cleanup ────────────────────────────────────────────────────────────────
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplNX_Shutdown();
    ImGui::DestroyContext();

    delete camPointer;
    delete blockModel;
    delete skyModel;
    if (world) delete world;

    deinitEGL();
    romfsExit();
    consoleExit(nullptr);
    return 0;
}
