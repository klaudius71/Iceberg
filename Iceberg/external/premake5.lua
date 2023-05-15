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

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        
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
        "%{VULKAN_SDK}/Include"
    }

    filter "system:windows"
        staticruntime "On"

        files
        {
            "imgui/backends/imgui_impl_glfw.h",
            "imgui/backends/imgui_impl_glfw.cpp",
            "imgui/backends/imgui_impl_vulkan.h",
            "imgui/backends/imgui_impl_vulkan.cpp"
        }
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
