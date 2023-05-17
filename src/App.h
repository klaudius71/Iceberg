#ifndef _APP
#define _APP

#include "IcebergCore.h"

namespace Iceberg {

	class Window;

	class ICEBERG_API App
	{
	private:
		static App* instance;
	protected:
		App(const int window_width = 1600, const int window_height = 900, const bool borderless = false, const char* const icon_path = nullptr);
		virtual ~App();
		App(const App&) = delete;
		App& operator=(const App&) = delete;
		App(App&&) = delete;
		App& operator=(App&&) = delete;

	public:
		void Run();

		virtual void Start() = 0;
		virtual void Update() = 0;
		virtual void End() = 0;

	private:
		static const char* const validationLayers[];
		static const std::vector<const char*> deviceExtensions;

		Window* window = nullptr;

		std::chrono::high_resolution_clock::time_point prevTime;
		float deltaTime;

	public:
		static const Window* GetWindow();
	};

}

#endif