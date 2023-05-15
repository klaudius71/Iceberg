workspace "Iceberg"
    configurations { "Debug", "Release" }
    platforms { "x86", "x64" }
    --defaultplatform "x86"
    startproject "Sandbox"
    flags { "MultiProcessorCompile" }
    warnings "Default"

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
VULKAN_SDK = os.getenv("VULKAN_SDK")

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

    libdirs
    {
        "%{VULKAN_SDK}/Lib"
    }

    links 
    { 
        "GLFW",
        "imgui",
        "vulkan-1.lib"
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
        "%{VULKAN_SDK}/Include",
        "Iceberg/external/glfw/include",
        "Iceberg/external/glm/include",
        "Iceberg/external/imgui",
        "Iceberg/external/stb"
    }

    filter { "platforms:x86" }
        architecture "x86"
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
        "%{VULKAN_SDK}/Include",
        "Iceberg/external/glfw/include",
        "Iceberg/external/glm/include",
        "Iceberg/external/imgui",
        "Iceberg/external/stb"
    }

    filter { "platforms:x86" }
        architecture "x86"
    filter { "platforms:x64" }
        architecture "x64"
    
    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
    
    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"