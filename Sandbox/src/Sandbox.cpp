#include "Sandbox.h"

Sandbox::Sandbox(const int width, const int height, const char* const icon_path)
	: App(width, height, true, icon_path), img1(nullptr), x_tex(nullptr), square_tex(nullptr), line_tex(nullptr), font(nullptr), pixels(nullptr)
{
}

void Sandbox::Start()
{
	img1 = new Iceberg::Image();
	x_tex = new Iceberg::Texture("assets/textures/x.png");
	square_tex = new Iceberg::Texture("assets/textures/square.png");
	line_tex = new Iceberg::Texture("assets/textures/line.png");
	minimize_tex = new Iceberg::Texture("assets/textures/minimize.png");
	pixels = new uint32_t[img1->GetWidth() * img1->GetHeight()];

	font = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 24);
}

void Sandbox::Update()
{
	glfwSetWindowCaptionArea(App::GetWindow()->GetGLFWWindow(), 0, 0, App::GetWindow()->GetWindowWidth() - 175, 30);

	const Iceberg::Window* wind = GetWindow();
	
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, 10.0f });
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::PushFont(font);
		ImGui::SetCursorPosY(-7.0f);
		ImGui::Text("Iceberg");
		ImGui::PopFont();
	
		float offset = ImGui::GetWindowWidth() - (ImGui::GetWindowHeight() + 20.0f + 4.0f);
		ImGui::SetCursorPosX(offset);
		ImGui::SetCursorPosY(-3.0f);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 20.0f, 10.0f });
		if (ImGui::ImageButton("Exit", x_tex->GetDescriptorSet(), ImVec2{ImGui::GetWindowHeight() * 0.6f , ImGui::GetWindowHeight() * 0.6f }))
			glfwSetWindowShouldClose(wind->GetGLFWWindow(), GLFW_TRUE);
	
		ImGui::SetCursorPosX(offset - (ImGui::GetWindowHeight() + 20.0f + 4.0f));
		ImGui::SetCursorPosY(-3.0f);
		ImTextureID tex = wind->IsMaximized() ? minimize_tex->GetDescriptorSet() : square_tex->GetDescriptorSet();
	
		if (ImGui::ImageButton("Maximize", tex, ImVec2{ ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f }))
		{
			if (wind->IsMaximized())
				glfwRestoreWindow(wind->GetGLFWWindow());
			else
				glfwMaximizeWindow(wind->GetGLFWWindow());
		}
	
		ImGui::SetCursorPosX(offset - 2 * (ImGui::GetWindowHeight() + 20.0f + 4.0f));
		ImGui::SetCursorPosY(-3.0f);
		if (ImGui::ImageButton("Minimize", line_tex->GetDescriptorSet(), ImVec2{ ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f }))
		{
			glfwIconifyWindow(wind->GetGLFWWindow());
		}
	
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	
		ImGui::EndMainMenuBar();
	}
	ImGui::PopStyleVar();
	
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
	ImGui::Begin("Image Example");
	ImVec2 window_size = ImGui::GetContentRegionAvail();
	this->Resize((uint32_t)window_size.x, (uint32_t)window_size.y);
	this->RenderFrame();
	ImGui::Image(img1->GetImGuiTexture(), window_size);
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::ShowDemoWindow();
}

void Sandbox::End()
{
	delete img1;
	delete x_tex;
	delete square_tex;
	delete line_tex;
	delete minimize_tex;
	delete pixels;
}

void Sandbox::RenderFrame()
{
	for (uint32_t y = 0; y < img1->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < img1->GetWidth(); x++)
		{
			const float fragCoords_x = ((2 * x) / (float)img1->GetWidth()) - 1.0f;
			const float fragCoords_y = ((2 * y) / (float)img1->GetHeight()) - 1.0f;

			glm::vec4 color = PerPixel(glm::vec2(-fragCoords_x, fragCoords_y));
			color.r = glm::clamp(color.r, 0.0f, 1.0f);
			color.g = glm::clamp(color.g, 0.0f, 1.0f);
			color.b = glm::clamp(color.b, 0.0f, 1.0f);

			pixels[y * img1->GetWidth() + x] = ConvertToRGBA(color);
		}
	}
	img1->SetData(pixels, img1->GetWidth() * img1->GetHeight() * sizeof(uint32_t));
}

uint32_t Sandbox::ConvertToRGBA(const glm::vec4& color)
{
	return (uint8_t)(color.a * 255.0f) << 24 | (uint8_t)(color.b * 255.0f) << 16 | (uint8_t)(color.g * 255.0f) << 8 | (uint8_t)(color.r * 255.0f);
}

glm::vec4 Sandbox::PerPixel(glm::vec2 fragCoords)
{
	return glm::vec4{ 0.5f * (fragCoords.y + 1.0f), 0.5f * (fragCoords.x + 1.0f) , 0.5f * (fragCoords.x + 1.0f) , 1.0f };
}

void Sandbox::Resize(uint32_t width, uint32_t height)
{
	if (img1->GetWidth() == width && img1->GetHeight() == height)
		return;

	img1->Resize(width, height);
	delete[] pixels;
	pixels = new uint32_t[img1->GetWidth() * img1->GetHeight()];
}