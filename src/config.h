#pragma once
#include "global.h"
#include <fstream>
#include <math.h>
#include <thread>
#include <mutex>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render/mesh.h"
#include "model/chunkBuilder.h"
#include "model/model.h"
#include "compat.h"

#include "world/world.h"
#include "world/block.h"

#define GL_MAJOR 3
#define GL_MINOR 3