#ifndef _APP
#define _APP

#include "IcebergCore.h"

#ifdef _DEBUG
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

namespace Iceberg {

	class Window;

	class ICEBERG_API App
	{
	private:
		static App* instance;
	protected:
		App(const int window_width = 1600, const int window_height = 900, const char* const icon_path = nullptr);
		virtual ~App();
		App(const App&) = delete;
		App& operator=(const App&) = delete;
		App(App&&) = delete;
		App& operator=(App&&) = delete;

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
		static VkResult CreateDebugUtilsMessengerEXT(VkInstance inst, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		static void DestroyDebugUtilsMessengerEXT(VkInstance inst, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		
		struct QueueFamilyIndices
		{
			uint32_t graphicsFamily;
			uint32_t computeFamily;
			uint32_t presentFamily;
			bool graphicsFamilyExists = false;
			bool computeFamilyExists = false;
			bool presentFamilyExists = false;
		};
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		static std::vector<const char*> GetRequiredExtensions();
		bool IsDeviceSuitable(VkPhysicalDevice device);

		bool CheckValidationLayerSupport() const;
		void SetupDebugMessenger();
		void ChooseVulkanDevice();
		void CreateLogicalDevice();
		void CreateSurface();
		void InitializeVulkan();
		void CleanupVulkan();

	public:
		void Run();

		virtual void Start() = 0;
		virtual void Update() = 0;
		virtual void End() = 0;

	private:
		static const char* const validationLayers[];

		Window* window = nullptr;
		VkInstance vulkanInstance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSurfaceKHR surface;

	public:
		static const Window* GetWindow();
	};

}

#endif