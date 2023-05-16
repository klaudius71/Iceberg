#include "ipch.h"
#include "Window.h"

namespace Iceberg {

	Window::Window(const int width, const int height, const bool borderless, const char* const icon_path)
		: prev_window_pos_x(0), prev_window_pos_y(0),
		prev_window_width(0), prev_window_height(0),
		window_width(width), window_height(height),
		maximized(false), mouse_click(false), dragging(false)
	{
		glfwSetErrorCallback(glfw_error_callback);

		int err = glfwInit();
		if (err == -1)
			throw std::exception("Failed to initialize GLFW!");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);

		window = glfwCreateWindow(width, height, "Iceberg", nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			throw std::exception("Failed to create a window!");
		}

		auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(window, (videoMode->width  - width) / 2, (videoMode->height  - height) / 2);

		SetWindowLongPtr(glfwGetWin32Window(window), GWL_STYLE, WS_SIZEBOX);
		SetWindowPos(glfwGetWin32Window(window), HWND_TOPMOST, 100, 100, width, height, SWP_SHOWWINDOW);

		//HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
		//SetClassLongPtr(glfwGetWin32Window(window), GCLP_HBRBACKGROUND, (LONG_PTR)brush);

		if (glfwVulkanSupported() == GLFW_FALSE)
		{
			glfwTerminate();
			throw std::exception("Vulkan support not found!");
		}

		hori_resize = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		vert_resize = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		nesw_resize = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
		nwse_resize = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, glfw_key_callback);
		glfwSetWindowMaximizeCallback(window, glfw_maximize_callback);
		glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
		glfwSetWindowSizeCallback(window, glfw_window_resize_callback);

		glfwGetWindowPos(window, &prev_window_pos_x, &prev_window_pos_y);

		if (icon_path)
			this->SetWindowIcon(icon_path);
	}
	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::Update()
	{
		//double xpos, ypos;
		//glfwGetCursorPos(window, &xpos, &ypos);
		//
		//static bool right = false;
		//static bool down = false;
		//static bool inArea = false;
		//static int window_x = 0;
		//static int window_y = 0;
		//static int window_width_prev = 0;
		//static int window_height_prev = 0;
		//if (!maximized && !dragging)
		//{
		//	right = xpos >= GetWindowWidth() - 6.0f;
		//	down = ypos >= GetWindowHeight() - 6.0f;
		//	inArea = right || down;
		//}
		//
		//mouse_click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		//static double prev_xpos = 0, prev_ypos = 0;
		//if (!mouse_click)
		//{
		//	dragging = false;
		//	prev_xpos = xpos;
		//	prev_ypos = ypos;
		//}
		//
		//if (inArea && mouse_click)
		//{
		//	if (!dragging)
		//	{
		//		glfwGetWindowPos(window, &window_x, &window_y);
		//		glfwGetWindowSize(window, &window_width_prev, &window_height_prev);
		//	}
		//	dragging = true;
		//}
		//
		//if (dragging)
		//{
		//	int delta_x = (int)(xpos - prev_xpos);
		//	int delta_y = (int)(ypos - prev_ypos);
		//
		//	int new_window_width = window_width_prev;
		//	int new_window_height = window_height_prev;
		//
		//	if (right)
		//		new_window_width += delta_x;
		//
		//	if (down)
		//		new_window_height += delta_y;
		//
		//	glfwSetWindowSize(window, new_window_width, new_window_height);
		//}
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
	bool Window::IsResizing() const
	{
		return dragging;
	}

	GLFWcursor* const Window::GetHoriResizeCursor() const
	{
		return hori_resize;
	}
	GLFWcursor* const Window::GetVertResizeCursor() const
	{
		return vert_resize;
	}
	GLFWcursor* const Window::GetNESWResizeCursor() const
	{
		return nesw_resize;
	}
	GLFWcursor* const Window::GetNWSEResizeCursor() const
	{
		return nwse_resize;
	}

	HWND Window::GetNativeWindow() const
	{
		return glfwGetWin32Window(window);
	}
	void Window::SetWindowIcon(const char* const icon_path)
	{
		int x, y, channels;
		uint8_t* img = stbi_load(icon_path, &x, &y, &channels, 0);
		assert(img);
		GLFWimage glfwImg{ x, y, img };
		glfwSetWindowIcon(window, 1, &glfwImg);
		stbi_image_free(img);
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

		//if (!wind->maximized && !wind->dragging)
		//{
		//	bool right = xpos >= wind->GetWindowWidth() - 6.0f;
		//	bool down = ypos >= wind->GetWindowHeight() - 6.0f;
		//
		//	if (right)
		//		if (down)
		//			glfwSetCursor(window, wind->GetNWSEResizeCursor());
		//		else
		//			glfwSetCursor(window, wind->GetHoriResizeCursor());
		//	else if (down)
		//		glfwSetCursor(window, wind->GetVertResizeCursor());
		//	else
		//		glfwSetCursor(window, NULL);
		//}
	}
	void Window::glfw_maximize_callback(GLFWwindow* window, int maximized)
	{
		Window* wind = (Window*)glfwGetWindowUserPointer(window);
		wind->maximized = maximized;
	}
	void Window::glfw_window_resize_callback(GLFWwindow* window, int width, int height)
	{
		if (width == 0 || height == 0)
			return;

		Window* wind = (Window*)glfwGetWindowUserPointer(window);
		wind->window_width = width;
		wind->window_height = height;
	}
}