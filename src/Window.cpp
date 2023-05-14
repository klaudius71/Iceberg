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
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		window = glfwCreateWindow(width, height, "Glacier V2", nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			throw std::exception("Failed to create a window!");
		}

		auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(window, (videoMode->width  - width) / 2, (videoMode->height  - height) / 2);

		//SetWindowLongPtr(glfwGetWin32Window(window), GWL_STYLE, WS_BORDER);
		//SetWindowPos(glfwGetWin32Window(window), HWND_TOPMOST, 100, 100, width, height, SWP_SHOWWINDOW);

		if (glfwVulkanSupported() == GLFW_FALSE)
		{
			glfwTerminate();
			throw std::exception("Vulkan support not found!");
		}

		hori_resize = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		vert_resize = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		fwd_resize = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
		bwd_resize = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, glfw_key_callback);
		glfwSetWindowMaximizeCallback(window, glfw_maximize_callback);
		glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);

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

	bool Window::IsMaximized() const
	{
		return maximized;
	}

	GLFWcursor* const Window::GetHoriResizeCursor() const
	{
		return hori_resize;
	}
	GLFWcursor* const Window::GetVertResizeCursor() const
	{
		return vert_resize;
	}
	GLFWcursor* const Window::GetFwdResizeCursor() const
	{
		return fwd_resize;
	}
	GLFWcursor* const Window::GetBwdResizeCursor() const
	{
		return bwd_resize;
	}

	HWND Window::GetNativeWindow() const
	{
		return glfwGetWin32Window(window);
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
	void Window::glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
	{
		//printf("%.2f %.2f\n", xpos, ypos);
		Window* wind = (Window*)glfwGetWindowUserPointer(window);

		bool left = xpos <= 6.0;
		bool right = xpos >= wind->GetWindowWidth() - 6.0f;
		bool up = ypos <= 6.0;
		bool down = ypos >= wind->GetWindowHeight() - 6.0f;

		if (!wind->maximized)
		{
			if (left)
				if (up)
					glfwSetCursor(window, wind->GetBwdResizeCursor());
				else if (down)
					glfwSetCursor(window, wind->GetFwdResizeCursor());
				else
					glfwSetCursor(window, wind->GetHoriResizeCursor());
			else if (right)
				if (up)
					glfwSetCursor(window, wind->GetFwdResizeCursor());
				else if (down)
					glfwSetCursor(window, wind->GetBwdResizeCursor());
				else
					glfwSetCursor(window, wind->GetHoriResizeCursor());
			else if (up || down)
				glfwSetCursor(window, wind->GetVertResizeCursor());
			else
				glfwSetCursor(window, NULL);
		}
		else
		{
			glfwSetCursor(window, NULL);
		}				
	}
	void Window::glfw_maximize_callback(GLFWwindow* window, int maximized)
	{
		Window* wind = (Window*)glfwGetWindowUserPointer(window);
		wind->maximized = maximized;
	}
}