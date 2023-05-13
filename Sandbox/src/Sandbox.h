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

private:
	Iceberg::Texture* tex1;
};

#endif // !_SANDBOX
