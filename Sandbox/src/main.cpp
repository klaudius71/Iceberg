#include "Sandbox.h"
#include <exception>

int main()
{
	try {
		Sandbox* sandbox = new Sandbox(1920, 1080, nullptr);
		sandbox->Run();
		delete sandbox;
	}
	catch (const std::exception& e) {
		MessageBoxA(NULL, e.what(), "Error", MB_ICONERROR | MB_OK);
	}
}