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
		App(const int window_width = 1600, const int window_height = 900, const char* const icon_path = nullptr);
		virtual ~App();
	private:
		App(const App&) = delete;
		App& operator=(const App&) = delete;
		App(App&&) = delete;
		App& operator=(App&&) = delete;

	public:
		void Run();

		virtual void Update() = 0;

	private:
		Window* window = nullptr;

	public:
		static const Window* GetWindow();
	};

}

#endif