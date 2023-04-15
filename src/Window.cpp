#include "ipch.h"
#include "Window.h"

namespace Iceberg {

	Window::Window(const int width, const int height, const char* const icon_path)
		: prev_window_pos_x(0), prev_window_pos_y(0),
		prev_window_width(0), prev_window_height(0),
		window_width(width), window_height(height)
	{
		glfwSetErrorCallback(glfw_error_callback);

		int err = glfwInit();
		if (err == -1)
			throw std::exception("Failed to initialize GLFW!");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, "Glacier V2", nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			throw std::exception("Failed to create a window!");
		}

		if (glfwVulkanSupported() == GLFW_FALSE)
		{
			glfwTerminate();
			throw std::exception("Vulkan support not found!");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, glfw_key_callback);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetWindowPos(window, &prev_window_pos_x, &prev_window_pos_y);

		if (icon_path)
			this->SetWindowIcon(icon_path);
	}
	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	const int Window::GetWindowWidth() const
	{
		return window_width;
	}
	const int Window::GetWindowHeight() const
	{
		return window_height;
	}
	GLFWwindow* const Window::GetGLFWWindow() const
	{
		return window;
	}
	void Window::SetWindowTitle(const char* const name) const
	{
		glfwSetWindowTitle(window, name);
	}
	void Window::HideCursor() const
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	void Window::ShowCursor() const
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	void Window::SetWindowIcon(const char* const icon_path)
	{
		UNREFERENCED_PARAMETER(icon_path);
		//int x, y, channels;
		//uint8_t* img = stbi_load(icon_path, &x, &y, &channels, 0);
		//assert(img);
		//GLFWimage glfwImg{ x, y, img };
		//glfwSetWindowIcon(window, 1, &glfwImg);
		//stbi_image_free(img);
	}
//	HWND Window::GetNativeWindow() const
//	{
//#if _WIN32
//		return glfwGetWin32Window(window);
//#else
//		return 0;
//#endif
//	}

	const bool Window::IsOpen()
	{
		return glfwWindowShouldClose(window);
	}
	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	void Window::glfw_error_callback(int error_code, const char* description)
	{
		UNREFERENCED_PARAMETER(error_code);
		printf(description);
		printf("\n");
	}
	void Window::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		UNREFERENCED_PARAMETER(scancode);
		UNREFERENCED_PARAMETER(mods);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}
}