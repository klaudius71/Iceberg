#ifndef _ICEBERG_CORE
#define _ICEBERG_CORE

#ifdef ICEBERG_BUILD_DLL
//#define ICEBERG_API __declspec(dllexport)
#define ICEBERG_API // Not using dynamic library at the moment
#else
//#define ICEBERG_API __declspec(dllimport)
#define ICEBERG_API // Not using dynamic library at the moment
#endif

#ifndef ICEBERG_VOLK
#define VULKAN_INCLUDE "vulkan/vulkan.h"
#ifdef _DEBUG
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif
#else
#define VULKAN_INCLUDE "volk.h"
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;
constexpr bool useVSync = true;

#endif