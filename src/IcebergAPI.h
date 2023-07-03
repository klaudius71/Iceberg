#ifndef _ICEBERG_API
#define _ICEBERG_API

#ifdef _WIN32
#include <windows.h>
#else
#define UNREFERENCED_PARAMETER(x) (x)
#endif

#include <cassert>
#include <cstdio>
#include <chrono>
#include <vector>

#include "IcebergCore.h"

#include VULKAN_INCLUDE
#include "../external/glfw/include/GLFW/glfw3.h"
//#include "../external/glfw/include/GLFW/glfw3native.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "../external/imgui/imgui.h"

#include "App.h"
#include "Window.h"
#include "Texture.h"
#include "Image.h"

#endif