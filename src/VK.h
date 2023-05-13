#ifndef _VK
#define _VK

#include "Vertex.h"
#include "IcebergCore.h"

namespace Iceberg {

#ifdef _DEBUG
	constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
	constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

	class VertexBuffer;
	class IndexBuffer;
	class UniformBuffer;
	class Pipeline;
	class Texture;

	class VK
	{
	private:
		static VK* instance;
		static VK& Instance()
		{
			assert(instance && "VK instance not initialized!");
			return *instance;
		}
		VK();
		VK(const VK&) = delete;
		VK& operator=(const VK&) = delete;
		VK(VK&&) = delete;
		VK& operator=(VK&&) = delete;
		~VK();

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
			//uint32_t transferFamily;
			uint32_t presentFamily;
			bool graphicsFamilyExists = false;
			bool computeFamilyExists = false;
			bool presentFamilyExists = false;
		};// queueFamilies;
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		// Swapchain
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice dev);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, bool vsync = false);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void CreateSwapChain();
		void CreateImageViews();
		void CleanupSwapChain();
		void RecreateSwapChain();

		static std::vector<const char*> GetRequiredExtensions();
		bool IsDeviceSuitable(VkPhysicalDevice device);

		bool CheckValidationLayerSupport() const;
		bool CheckDeviceExtensionSupport(VkPhysicalDevice dev) const;
		void SetupDebugMessenger();

		void InitializeVulkan();

		void ChooseVulkanDevice();
		void CreateLogicalDevice();

		void CreateSurface();

		void CreateRenderPass();
		void CreateDescriptorSetLayouts();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateGraphicsPipeline();

		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateCommandBuffer();

		void CreateSyncObjects();

		VkDescriptorPool imguiPool;
		void initializeImGui();
		void terminateImGui();

		void updateUniforms();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void drawFrame();
		void deviceWaitIdle();

		void CleanupVulkan();

		static const char* const validationLayers[];
		static const std::vector<const char*> deviceExtensions;

		VkInstance vkInstance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSurfaceKHR surface;

		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<VkImageView> swapChainImageViews;

		VkRenderPass renderPass;
		VertexBuffer* vertexBuffer;
		IndexBuffer* indexBuffer;
		VkDescriptorSetLayout descriptorSetLayoutUniform;
		VkDescriptorSetLayout descriptorSetLayoutSampler;
		UniformBuffer* cameraUniformBuffers;
		UniformBuffer* worldUniformBuffers;
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
		Pipeline* graphicsPipeline;

		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT];
		VkSemaphore imageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
		VkSemaphore renderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
		VkFence inFlightFence[MAX_FRAMES_IN_FLIGHT];

		uint32_t currentFrame = 0;

		const std::vector<Vertex> vertices
		{
			{{ -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
			{{  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
			{{  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }},
			{{ -0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }}
		};
		const std::vector<uint32_t> indices
		{
			0, 1, 2,
			2, 3, 0
		};

		Texture* crateTexture;

	public:
		static void Initialize();
		static VkInstance GetVkInstance();
		static void InitializeImGui() { Instance().initializeImGui(); }
		static void TerminateImGui() { Instance().terminateImGui(); }
		static VkDevice GetLogicalDevice();
		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		static VkCommandPool GetCommandPool();
		static VkQueue GetTransferQueue();
		static VkDescriptorPool GetDescriptorPool();
		static uint32_t GetCurrentFrame();
		static VkDescriptorSetLayout GetDescriptorSetLayoutSampler();
		static void DrawFrame() { Instance().drawFrame(); }
		static void DeviceWaitIdle() { Instance().deviceWaitIdle(); }
		static void Terminate();

		struct Helper
		{
			static VkCommandBuffer BeginOneTimeCommand();
			static void EndOneTimeCommand(VkCommandBuffer commandBuffer);
		};
	};

}

#endif