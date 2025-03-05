#pragma once
#include "global.h"
#include "helper.h"
#include "compat.h"
#include <fstream>
#include <math.h>
#include <thread>
#include <mutex>
#include <fmt/core.h>
#include "version.h"

#include "include/imgui/imgui.h"
#include "include/imgui/backends/imgui_impl_glfw.h"
#include "include/imgui/backends/imgui_impl_opengl3.h"
#include "include/stb/stb_image_write.h"


#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render/mesh.h"
#include "render/sky.h"
#include "model/chunkBuilder.h"
#include "model/model.h"

#include "world/world.h"
#include "world/block.h"
#include "world/blockProperties.h"

#define GL_MAJOR 3
#define GL_MINOR 3