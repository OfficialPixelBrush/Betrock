#pragma once
#include "global.h"
#include <fstream>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <imgui/imgui.h>
//#include <imgui/backends/imgui_impl_glfw.h>
//#include <imgui/backends/imgui_impl_opengl3.h>
#include "render/shader.h"
#include "render/vao.h"
#include "render/vbo.h"
#include "render/ebo.h"
#include "render/texture.h"
#include "render/camera.h"
#include "compat.h"

#include "world/region.h"

#define GL_MAJOR 3
#define GL_MINOR 3