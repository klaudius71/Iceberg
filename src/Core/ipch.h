#pragma once

#if _WIN32
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#else
#define UNREFERENCED_PARAMETER(x) (x)
#endif

#include "IcebergCore.h"

#include <cassert>
#include <cstdio>
#include <chrono>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <array>

#include VULKAN_INCLUDE
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_INTRINSICS
#define GLM_FORCE_SSE42
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL // glm::length2() is experimental for some reason
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "stb_image.h"

//#include "Colors.h"

//#pragma warning(disable : 4005)
//#include "../external/MemTracker/Framework.h"