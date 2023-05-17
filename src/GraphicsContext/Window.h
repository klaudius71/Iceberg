#ifndef _WINDOW
#define _WINDOW

#include "IcebergCore.h"

namespace Iceberg {

	class ICEBERG_API Window
	{
	public:
		Window(const int width, const int height, const bool borderless, const char* const icon_path);
		Window() = delete;
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;
		virtual ~Window();

		void Update();

		const int GetWindowWidth() const;
		const int GetWindowHeight() const;
		GLFWwindow* const GetGLFWWindow() const;
		void SetWindowTitle(const char* const name) const;
		void HideCursor() const;
		void ShowCursor() const;
		bool IsMaximized() const;
		bool IsResizing() const;

		GLFWcursor* const GetHoriResizeCursor() const;
		GLFWcursor* const GetVertResizeCursor() const;
		GLFWcursor* const GetNESWResizeCursor() const;
		GLFWcursor* const GetNWSEResizeCursor() const;

		void SetWindowIcon(const char* const icon_path);
		HWND GetNativeWindow() const;

		const bool IsOpen();
		void PollEvents();

		//virtual void SetClearColor(const float red, const float green, const float blue, const float alpha = 1.0f) const;
		//virtual void Clear();
		//virtual void SwapBuffers();

	protected:
		static void glfw_error_callback(int error_code, const char* description);
		static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
		static void glfw_maximize_callback(GLFWwindow* window, int maximized);
		static void glfw_window_resize_callback(GLFWwindow* window, int width, int height);

		int prev_window_pos_x;
		int prev_window_pos_y;
		int prev_window_width;
		int prev_window_height;
		int window_width;
		int window_height;
		GLFWwindow* window;

		GLFWcursor* hori_resize;
		GLFWcursor* vert_resize;
		GLFWcursor* nesw_resize;
		GLFWcursor* nwse_resize;

		bool maximized;
		bool mouse_click;
		bool dragging;
	};

}

#endif // !_WINDOW
