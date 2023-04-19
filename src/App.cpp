#include "ipch.h"
#include "App.h"
#include "Window.h"
#include "VK.h"

namespace Iceberg {

	App* App::instance = nullptr;

	App::App(const int window_width, const int window_height, const char* const icon_path)
		: window(nullptr)
	{
		assert(instance == nullptr);
		instance = this;

		window = new Window(window_width, window_height, icon_path);
		VK::Initialize();
	}
	App::~App()
	{
		VK::Terminate();
		delete window;
	}

	void App::Run()
	{
		Start();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		//ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindow(), true);
		//ImGui_ImplVulkan_Init();

		while (!window->IsOpen())
		{
			window->PollEvents();
			
			//ImGui_ImplGlfw_NewFrame();
			//ImGui_ImplVulkan_NewFrame();
			//ImGui::NewFrame();
			//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
			
			Update();

			VK::DrawFrame();

			//ImGui::Render();
			//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), , nullptr);
		}

		VK::DeviceWaitIdle();

		// Cleanup
		//ImGui_ImplVulkan_Shutdown();
		//ImGui_ImplGlfw_Shutdown();
		//ImGui::DestroyContext();

		End();
	}

	const Window* App::GetWindow()
	{
		assert(instance && "Application instance not set!");
		return instance->window;
	}

}

