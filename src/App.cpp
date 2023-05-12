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
		// io.ConfigViewportsNoAutoMerge = true;
		// io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsLight();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		VK::InitializeImGui();

		char buf[64];
		int count = 0;
		while (!window->IsOpen())
		{
			auto currTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> duration = currTime - prevTime;
			deltaTime = duration.count() * 1000.0f;
			prevTime = currTime;
			count++;
			if (count == 10)
			{
				count = 0;
				sprintf_s(buf, "woohoo, it works    deltaTime: %.3fms", deltaTime);
				window->SetWindowTitle(buf);
			}

			window->PollEvents();
			
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
			
			//ImGui::ShowDemoWindow();

			Update();

			ImGui::Render();
			VK::DrawFrame();

			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		VK::DeviceWaitIdle();

		// Cleanup
		VK::TerminateImGui();
		ImGui::DestroyContext();

		End();
	}

	const Window* App::GetWindow()
	{
		assert(instance && "Application instance not set!");
		return instance->window;
	}

}

