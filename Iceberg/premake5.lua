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
        "include",
        "src",
        "src/Core",
        "src/GraphicsContext",
        "src/GraphicsContext/Buffers",
        "src/GraphicsContext/Pipeline",
        "src/GraphicsContext/Texture",
        "%{VULKAN_INCLUDE}",
        "external/glfw/include",
        "external/glm",
        "external/imgui",
        "external/stb"
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