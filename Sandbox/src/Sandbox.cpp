#include "Sandbox.h"

Sandbox::Sandbox(const int width, const int height, const char* const icon_path)
	: App(width, height, icon_path), img1(nullptr), pixels(nullptr)
{
}

void Sandbox::Start()
{
	img1 = new Iceberg::Image();
	pixels = new uint32_t[img1->GetWidth() * img1->GetHeight()];
}

void Sandbox::Update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
	ImGui::Begin("Heyyy");
	ImVec2 window_size = ImGui::GetContentRegionAvail();
	this->Resize((uint32_t)window_size.x / 2, (uint32_t)window_size.y / 2);
	this->RenderFrame();
	ImGui::Image(img1->GetImGuiTexture(), window_size);
	ImGui::End();
	ImGui::PopStyleVar();
}

void Sandbox::End()
{
	delete img1;
	delete pixels;
}

void Sandbox::RenderFrame()
{
	for (uint32_t i = 0; i < img1->GetHeight(); i++)
	{
		for (uint32_t j = 0; j < img1->GetWidth(); j++)
		{
			pixels[i * img1->GetWidth() + j] = ConvertToRGBA(PerPixel(j, i));
		}
	}
	img1->SetData(pixels, img1->GetWidth() * img1->GetHeight() * sizeof(uint32_t));
}

uint32_t Sandbox::ConvertToRGBA(const glm::vec4& color)
{
	return (uint8_t)(color.a * 255.0f) << 24 | (uint8_t)(color.b * 255.0f) << 16 | (uint8_t)(color.g * 255.0f) << 8 | (uint8_t)(color.r * 255.0f);
}

glm::vec4 Sandbox::PerPixel(uint32_t x, uint32_t y)
{
	return glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };
	//return glm::vec4{ (float)rand() / RAND_MAX, (float)rand() / RAND_MAX , (float)rand() / RAND_MAX , 1.0f };
}

void Sandbox::Resize(uint32_t width, uint32_t height)
{
	if (img1->GetWidth() == width && img1->GetHeight() == height)
		return;

	img1->Resize(width, height);
	delete[] pixels;
	pixels = new uint32_t[img1->GetWidth() * img1->GetHeight()];
}
