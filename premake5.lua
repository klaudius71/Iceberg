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
newoption { 
    trigger = "include-sandbox",
    value = "sand",
    description = "Include the Sandbox project",
    allowed = {
        { "no", "No" },
        { "yes", "Yes" }
    },
    default = "no"
}

workspace "Iceberg"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    startproject "Sandbox"
    flags { "MultiProcessorCompile" }
    warnings "Default"

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
    include "external"
group ""

project "Iceberg"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "ipch.h"
    pchsource "src/Core/ipch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    links 
    { 
        "GLFW",
        "imgui",
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
    
    filter { "options:VulkanSDK=sdk" }
        includedirs { "%{VULKAN_SDK}/Include" }
        libdirs { "%{VULKAN_SDK}/Lib" }
        links { "vulkan-1.lib" }

    filter { "options:VulkanSDK=volk" }
        includedirs 
        { 
            "external/volk",
            "external/Vulkan-Headers/include"
        }
        links { "volk" }
        defines { "ICEBERG_VOLK" }

    filter { "platforms:x64" }
        architecture "x64"

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
    
    filter { "configurations:Release" }
        defines {"NDEBUG"}
        optimize "On"

if _OPTIONS["include-sandbox"] == "yes" then
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
end