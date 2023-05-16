workspace "Iceberg"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    startproject "Sandbox"
    flags { "MultiProcessorCompile" }
    warnings "Default"

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

newoption {
    trigger = "VulkanSDK",
    value = "API",
    description = "Choose how to load Vulkan",
    allowed = {
       { "sdk",    "SDK" },
       { "volk",  "volk" }
    },
    default = "sdk"
 }

if _OPTIONS["VulkanSDK"] == "volk" then
    VULKAN_HEADERS = "%{wks.location}/Iceberg/external/Vulkan-Headers/include"
    VULKAN_INCLUDE = "%{wks.location}/Iceberg/external/volk"
else
    VULKAN_SDK = os.getenv("VULKAN_SDK")
    VULKAN_HEADERS = "%{VULKAN_SDK}/Include"
    VULKAN_INCLUDE = "%{VULKAN_SDK}/Include"
end

group "Dependencies"
    include "Iceberg/external"
group ""

project "Iceberg"
    location "Iceberg"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "ipch.h"
    pchsource "Iceberg/src/Core/ipch.cpp"

    files
    {
        "Iceberg/src/**.h",
        "Iceberg/src/**.cpp"
    }

    links 
    { 
        "GLFW",
        "imgui",
    }

    includedirs 
    { 
        "Iceberg/include",
        "Iceberg/src",
        "Iceberg/src/Core",
        "Iceberg/src/GraphicsContext",
        "Iceberg/src/GraphicsContext/Buffers",
        "Iceberg/src/GraphicsContext/Pipeline",
        "Iceberg/src/GraphicsContext/Texture",
        "%{VULKAN_INCLUDE}",
        "Iceberg/external/glfw/include",
        "Iceberg/external/glm/include",
        "Iceberg/external/imgui",
        "Iceberg/external/stb"
    }
    
    filter { "options:VulkanSDK=sdk" }
        libdirs { "%{VULKAN_SDK}/Lib" }
        links { "vulkan-1.lib" }

    filter { "options:VulkanSDK=volk" }
        links { "volk" }
        defines { "ICEBERG_VOLK" }
        includedirs { "%{VULKAN_HEADERS}" }

    filter { "platforms:x64" }
        architecture "x64"

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
    
    filter { "configurations:Release" }
        defines {"NDEBUG"}
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    debugdir "%{wks.location}"

    files
    {
        "Sandbox/src/**.h",
        "Sandbox/src/**.cpp"
    }

    links 
    { 
        "GLFW",
        "imgui",
        "Iceberg"
    }

    includedirs 
    { 
        "Iceberg/src",
        "Iceberg/src/Core",
        "Iceberg/src/GraphicsContext",
        "Iceberg/src/GraphicsContext/Buffers",
        "Iceberg/src/GraphicsContext/Pipeline",
        "Iceberg/src/GraphicsContext/Texture",
        "%{VULKAN_INCLUDE}",
        "Iceberg/external/glfw/include",
        "Iceberg/external/glm/include",
        "Iceberg/external/imgui",
        "Iceberg/external/stb"
    }

    filter { "options:VulkanSDK=volk" }
        defines { "ICEBERG_VOLK" }
        includedirs { "%{VULKAN_HEADERS}" }

    filter { "platforms:x64" }
        architecture "x64"
    
    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
    
    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"