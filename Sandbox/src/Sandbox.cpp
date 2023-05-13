#include "Sandbox.h"

Sandbox::Sandbox(const int width, const int height, const char* const icon_path)
	: App(width, height, icon_path)
{
}

void Sandbox::Start()
{
	tex1 = new Iceberg::Texture("assets/textures/crate_diffuse.tga");
}

void Sandbox::Update()
{
	ImGui::Begin("Heyyy");
	ImGui::Text("That's pretty cool!");
	ImGui::Image(tex1->GetDescriptorSet(), ImVec2{ 256, 256 });
	ImGui::End();
}

void Sandbox::End()
{
	delete tex1;
}
