workspace "Iceberg"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    startproject "Sandbox"
    flags { "MultiProcessorCompile" }
    warnings "Default"

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
    include "Iceberg/external"
group ""

include "Iceberg"

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
        "Iceberg/external/glm",
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