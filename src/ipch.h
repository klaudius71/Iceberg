#pragma once

#if _WIN32
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <cassert>
#include <cstdio>
#include <chrono>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdint>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_INTRINSICS
#define GLM_FORCE_SSE42
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL // glm::length2() is experimental for some reason
#include "glm.hpp"
#include "ext.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

//#include "Colors.h"

//#pragma warning(disable : 4005)
//#include "../external/MemTracker/Framework.h"