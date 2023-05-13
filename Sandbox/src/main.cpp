#include "Sandbox.h"
#include <exception>

int main()
{
	Sandbox* sandbox = new Sandbox(1600, 900, nullptr);
	sandbox->Run();
	delete sandbox;
}