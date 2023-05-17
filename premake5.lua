workspace "Iceberg"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    startproject "Sandbox"
    flags { "MultiProcessorCompile" }
    warnings "Default"

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "Iceberg.lua"

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
        "Iceberg"
    }

    includedirs 
    { 
        "src",
        "src/Core",
        "src/GraphicsContext",
        "src/GraphicsContext/Buffers",
        "src/GraphicsContext/Pipeline",
        "src/GraphicsContext/Texture",
        "external/glfw/include",
        "external/glm",
        "external/imgui",
        "external/stb"
    }

    filter { "options:VulkanSDK=volk" }
        defines { "ICEBERG_VOLK" }
        includedirs 
        { 
            "external/volk",
            "external/Vulkan-Headers/include" 
        }

    filter { "platforms:x64" }
        architecture "x64"
    
    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
    
    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"