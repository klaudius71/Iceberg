#ifndef _SANDBOX
#define _SANDBOX

#include "IcebergAPI.h"

class Sandbox : public Iceberg::App
{
public:
	Sandbox(const int width, const int height, const char* const icon_path);
	Sandbox(const Sandbox&) = delete;
	Sandbox& operator=(const Sandbox&) = delete;
	Sandbox(Sandbox&&) = delete;
	Sandbox& operator=(Sandbox&&) = delete;
	~Sandbox() = default;

	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;

	void RenderFrame();

	static uint32_t ConvertToRGBA(const glm::vec4& color);

private:
	glm::vec4 PerPixel(uint32_t x, uint32_t y);
	void Resize(uint32_t width, uint32_t height);

	uint32_t* pixels;
	Iceberg::Image* img1;
};

#endif // !_SANDBOX
