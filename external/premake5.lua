project "GLFW"
    location "glfw"
    kind "StaticLib"
    language "C"
    warnings "Default"

	targetname "glfw3"
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
	{
		"glfw/include/GLFW/glfw3.h",
		"glfw/include/GLFW/glfw3native.h",     
        "glfw/src/context.c",
        "glfw/src/init.c",
        "glfw/src/input.c",
        "glfw/src/monitor.c",
        "glfw/src/null_init.c",
        "glfw/src/null_joystick.c",
        "glfw/src/null_monitor.c",
        "glfw/src/null_window.c",
        "glfw/src/platform.c",
        "glfw/src/vulkan.c",
        "glfw/src/window.c"
	}
    
	filter "system:windows"
		staticruntime "On"

		files
		{
			"glfw/src/win32_init.c",
			"glfw/src/win32_joystick.c",
            "glfw/src/win32_module.c",
			"glfw/src/win32_monitor.c",
			"glfw/src/win32_thread.c",
			"glfw/src/win32_time.c",
			"glfw/src/win32_window.c",
			"glfw/src/wgl_context.c",
			"glfw/src/egl_context.c",
			"glfw/src/osmesa_context.c"
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "On"

        files
        {
            "glfw/src/x11_init.c",
            "glfw/src/x11_monitor.c",
            "glfw/src/x11_window.c",
            "glfw/src/xkb_unicode.c",
            "glfw/src/posix_time.c",
            "glfw/src/posix_module.c",
            "glfw/src/posix_thread.c",
            "glfw/src/posix_poll.c",
            "glfw/src/glx_context.c",
            "glfw/src/egl_context.c",
            "glfw/src/osmesa_context.c",
            "glfw/src/linux_joystick.c"
        }

        defines
        {
            "_GLFW_X11"
        }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        
if _OPTIONS["VulkanSDK"] == "volk" then
project "volk"
    location "volk"
    kind "StaticLib"
    language "C"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
	{
		"volk/volk.h",
        "volk/volk.c"
	}

	includedirs
	{
		"Vulkan-Headers/include",
	}

	filter "system:windows"
		defines { "VK_USE_PLATFORM_WIN32_KHR" }

	filter "system:linux"
		defines { "VK_USE_PLATFORM_XLIB_KHR" }

	filter "system:macosx"
		defines { "VK_USE_PLATFORM_MACOS_MVK" }

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

end

project "imgui"
    location "imgui"
    kind "StaticLib"
    language "C++"
    warnings "Default"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "imgui/*.h",
        "imgui/*.cpp"
    }

    includedirs
    {
        "imgui",
        "glfw/include",
    }

    files
    {
        "imgui/backends/imgui_impl_glfw.h",
        "imgui/backends/imgui_impl_glfw.cpp",
        "imgui/backends/imgui_impl_vulkan.h",
        "imgui/backends/imgui_impl_vulkan.cpp"
    }
    
    filter "system:windows"
        staticruntime "On"
    
    filter "system:linux"
        pic "On"
        staticruntime "On"

    filter "options:VulkanSDK=sdk"
        includedirs { "%{VULKAN_SDK}/Include" }

    filter "options:VulkanSDK=volk"
        includedirs { "Vulkan-Headers/include" }
        defines { "IMGUI_IMPL_VULKAN_NO_PROTOTYPES" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
