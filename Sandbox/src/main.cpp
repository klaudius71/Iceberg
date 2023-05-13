#include "Sandbox.h"
#include <exception>

struct MemMarker
{
	~MemMarker()
	{

	}
} mem_marker;

int main()
{
	Sandbox* sandbox = new Sandbox(1600, 900, nullptr);
	sandbox->Run();
	delete sandbox;
}