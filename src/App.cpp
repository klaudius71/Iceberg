#include "ipch.h"
#include "App.h"
#include "Window.h"

namespace Iceberg {

	App* App::instance = nullptr;

	App::App(const int window_width, const int window_height, const char* const icon_path)
	{
		assert(instance == nullptr);
		instance = this;

		window = new Window(window_width, window_height, icon_path);
	}
	App::~App()
	{
		delete window;
	}

	void App::Run()
	{
		while (!window->IsOpen())
		{
			window->PollEvents();
			Update();
		}
	}

	const Window* App::GetWindow()
	{
		assert(instance && "Application instance not set!");
		return instance->window;
	}

}

