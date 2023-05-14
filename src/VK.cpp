#include "ipch.h"
#include "VK.h"
#include "App.h"
#include "Window.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "StagingBuffer.h"
#include "UniformBuffer.h"
#include "Pipeline.h"
#include "Texture.h"
#include "DescriptorSet.h"

namespace Iceberg {

	VK* VK::instance = nullptr;
	const char* const VK::validationLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};
	const std::vector<const char*> VK::deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	void VK::Initialize()
	{
		assert(!instance && "VK instance already created!");
		instance = (VK*)malloc(sizeof(VK));
		memset(instance, 0, sizeof(VK));
		new(instance) VK;
	}
	VkInstance VK::GetVkInstance()
	{
		return Instance().vkInstance;
	}
	VkDevice VK::GetLogicalDevice()
	{
		return Instance().device;
	}
	uint32_t VK::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(Instance().physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		throw std::exception("Failed to find suitable memory type!");

		return 0;
	}
	VkCommandPool VK::GetCommandPool()
	{
		return Instance().commandPool;
	}
	VkQueue VK::GetTransferQueue()
	{
		return Instance().graphicsQueue;
	}
	VkDescriptorPool VK::GetDescriptorPool()
	{
		return Instance().descriptorPool;
	}
	uint32_t VK::GetCurrentFrame()
	{
		return Instance().currentFrame;
	}
	VkDescriptorSetLayout VK::GetDescriptorSetLayoutSampler()
	{
		return Instance().descriptorSetLayoutSampler;
	}
	void VK::Terminate()
	{
		assert(instance && "VK instance not created!");
		instance->~VK();
		free(instance);
		instance = nullptr;
	}

	VK::VK()
	{
		InitializeVulkan();
		SetupDebugMessenger();
		CreateSurface();
		ChooseVulkanDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateCommandPool();
		CreateDescriptorPool();
		CreateDescriptorSetLayouts();

		crateTexture = new Texture("assets/textures/crate_diffuse.tga");
		
		cameraUniformBuffer = new UniformBuffer(device, sizeof(glm::mat4) * 2);
		worldUniformBuffer = new UniformBuffer(device, sizeof(glm::mat4));

		CreateDescriptorSets();
		CreateGraphicsPipeline();
		CreateFramebuffers();

		// Vertex buffer
		uint64_t bufferSize = sizeof(Vertex) * vertices.size();
		vertexBuffer = new VertexBuffer(device, bufferSize);
		StagingBuffer stagingBuffer(device, bufferSize);
		void* mem;
		stagingBuffer.Map(mem);
		memcpy_s(mem, bufferSize, vertices.data(), bufferSize);
		stagingBuffer.Unmap();
		stagingBuffer.TransferBuffer(vertexBuffer);

		// Index buffer
		bufferSize = sizeof(uint32_t) * indices.size();
		indexBuffer = new IndexBuffer(device, (uint32_t)indices.size());
		stagingBuffer.Resize(bufferSize);
		stagingBuffer.Map(mem);
		memcpy_s(mem, bufferSize, indices.data(), bufferSize);
		stagingBuffer.Unmap();
		stagingBuffer.TransferBuffer(indexBuffer);

		CreateCommandBuffer();
		CreateSyncObjects();
	}
	VK::~VK()
	{
		CleanupVulkan();
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VK::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
	{
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			printf("%s\n", pCallbackData->pMessage);

		return VK_FALSE;
	}
	VkResult VK::CreateDebugUtilsMessengerEXT(VkInstance inst, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		static auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(inst, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(inst, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	void VK::DestroyDebugUtilsMessengerEXT(VkInstance inst, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		static auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(inst, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(inst, debugMessenger, pAllocator);
	}
	VK::QueueFamilyIndices VK::FindQueueFamilies(VkPhysicalDevice dev)
	{
		QueueFamilyIndices indices{};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
				indices.graphicsFamilyExists = true;
			}
			if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				indices.computeFamilyExists = true;
				indices.computeFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &presentSupport);
			if (presentSupport)
			{
				indices.presentFamily = i;
				indices.presentFamilyExists = true;
			}
		}

		return indices;
	}

	// Swapchain
	VK::SwapChainSupportDetails VK::QuerySwapChainSupport(VkPhysicalDevice dev)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
	VkSurfaceFormatKHR VK::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		if (availableFormats.empty())
			throw std::exception("No available swapchain surface formats available!");

		//for (const auto& availableFormat : availableFormats)
		//{
		//	if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		//		return availableFormat;
		//}

		return availableFormats[0];
	}
	VkPresentModeKHR VK::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, bool vsync)
	{
		if (vsync)
		{
			printf("\npresent mode: %s\n", "VK_PRESENT_MODE_FIFO_KHR");
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				printf("\npresent mode: %s\n", "VK_PRESENT_MODE_MAILBOX_KHR");
				return availablePresentMode;
			}
		}

		printf("\npresent mode: %s\n", "VK_PRESENT_MODE_FIFO_KHR");
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VK::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;
		else
		{
			int width, height;
			glfwGetFramebufferSize(App::GetWindow()->GetGLFWWindow(), &width, &height);
			return VkExtent2D{ std::clamp((uint32_t)width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
								std::clamp((uint32_t)height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height) };
		}
	}
	void VK::CreateSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes, useVSync);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };
		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult res = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create a swap chain!");

		//uint32_t imageCount;
		res = vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		assert(res == VK_SUCCESS);
		swapChainImages.resize(imageCount);
		res = vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
		assert(res == VK_SUCCESS);

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}
	void VK::CreateImageViews()
	{
		swapChainImageViews.resize(swapChainImages.size());

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			createInfo.image = swapChainImages[i];
			VkResult res = vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]);
			if (res != VK_SUCCESS)
				throw std::exception("Failed to create image views!");
		}
	}
	void VK::CleanupSwapChain()
	{
		for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
			vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
			vkDestroyImageView(device, swapChainImageViews[i], nullptr);

		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}
	void VK::RecreateSwapChain()
	{
		int width, height;
		glfwGetFramebufferSize(App::GetWindow()->GetGLFWWindow(), &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(App::GetWindow()->GetGLFWWindow(), &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);

		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
		CreateFramebuffers();
	}

	std::vector<const char*> VK::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (ENABLE_VALIDATION_LAYERS)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}
	bool VK::IsDeviceSuitable(VkPhysicalDevice dev)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(dev, &deviceProperties);
		vkGetPhysicalDeviceFeatures(dev, &deviceFeatures);

		bool extensionsSupported = CheckDeviceExtensionSupport(dev);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(dev);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		QueueFamilyIndices queueFamilies = FindQueueFamilies(dev);
		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
			&& deviceFeatures.geometryShader
			&& queueFamilies.graphicsFamilyExists
			&& queueFamilies.computeFamilyExists
			&& queueFamilies.presentFamilyExists
			&& extensionsSupported
			&& swapChainAdequate
			&& deviceFeatures.samplerAnisotropy == VK_TRUE;
	}

	bool VK::CheckValidationLayerSupport() const
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const auto& layerName : validationLayers)
			for (const auto& layerProperties : availableLayers)
				if (strcmp(layerName, layerProperties.layerName) == 0)
					return true;

		return false;
	}
	bool VK::CheckDeviceExtensionSupport(VkPhysicalDevice dev) const
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(dev, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(dev, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.cbegin(), deviceExtensions.cend());
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}
	void VK::SetupDebugMessenger()
	{
		if (!ENABLE_VALIDATION_LAYERS)
			return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;

		VkResult res = CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &debugMessenger);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to set up debug messenger!");
	}
	
	void VK::InitializeVulkan()
	{
		if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
			throw std::exception("Validation layers requested, but not available!");

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Iceberg";
		appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 1);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (ENABLE_VALIDATION_LAYERS)
		{
			createInfo.enabledLayerCount = 1;
			createInfo.ppEnabledLayerNames = validationLayers;

			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCreateInfo.pfnUserCallback = debugCallback;
			createInfo.pNext = &debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		std::vector<const char*> glfwExtensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = (uint32_t)glfwExtensions.size();
		createInfo.ppEnabledExtensionNames = glfwExtensions.data();
		VkResult result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
		if (result != VK_SUCCESS)
			throw std::exception("Failed to create a Vulkan instance!");

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		printf("Available Vulkan extensions:\n");
		for (uint32_t i = 0; i < extensionCount; i++)
			printf("\t%s\n", extensions[i].extensionName);
	}

	void VK::ChooseVulkanDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
		if (deviceCount <= 0)
			throw std::exception("Could not find GPUs that support Vulkan!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

		VkPhysicalDeviceProperties properties;
		printf("\nSupported GPUs:\n");
		for (const auto dev : devices)
		{
			vkGetPhysicalDeviceProperties(dev, &properties);
			printf("\t%s", properties.deviceName);
			if (physicalDevice == VK_NULL_HANDLE && IsDeviceSuitable(dev))
			{
				printf(" << selected");
				physicalDevice = dev;
			}
			printf("\n");
		}

		if (physicalDevice == VK_NULL_HANDLE)
			throw std::exception("Failed to find suitable GPU!");
	}
	void VK::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		uint32_t uniqueQueueFamilies[]{ indices.graphicsFamily, indices.presentFamily };
		std::sort(uniqueQueueFamilies, &uniqueQueueFamilies[1]);

		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo queueCreateInfo{};
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.emplace_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
		createInfo.pEnabledFeatures = &deviceFeatures;

		if constexpr (ENABLE_VALIDATION_LAYERS)
		{
			createInfo.enabledLayerCount = 1;
			createInfo.ppEnabledLayerNames = validationLayers;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		VkResult res = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create logical device!");

		vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
	}
	
	void VK::CreateSurface()
	{
		VkResult res = glfwCreateWindowSurface(vkInstance, App::GetWindow()->GetGLFWWindow(), nullptr, &surface);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create window surface!");
	}

	void VK::CreateRenderPass()
	{
		// Attachment description
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Subpasses and attachment references
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0; // layout(location = 0) out vec4 outColor
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		// Render pass
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult res = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create render pass!");
	}
	void VK::CreateDescriptorSetLayouts()
	{
		const VkDescriptorSetLayoutBinding layoutBinding[]
		{
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
			{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
		};
		const VkDescriptorSetLayoutBinding layoutBinding2[]
		{
			{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }
		};

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = (uint32_t)std::size(layoutBinding);
		layoutInfo.pBindings = layoutBinding;
		VkResult res;
		res = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayoutUniform);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create descriptor set layout!");

		layoutInfo.bindingCount = (uint32_t)std::size(layoutBinding2);
		layoutInfo.pBindings = layoutBinding2;
		res = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayoutSampler);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create descriptor set layout!");
	}
	void VK::CreateDescriptorPool()
	{
		VkDescriptorPoolSize poolSize[]
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 * (uint32_t)MAX_FRAMES_IN_FLIGHT },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 * (uint32_t)MAX_FRAMES_IN_FLIGHT }
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = (uint32_t)std::size(poolSize);
		poolInfo.pPoolSizes = poolSize;
		poolInfo.maxSets = 1000 * (uint32_t)MAX_FRAMES_IN_FLIGHT;
		
		VkResult res = vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create uniform buffer descriptor pool!");
	}
	void VK::CreateDescriptorSets()
	{
		descriptorSet = new DescriptorSet(descriptorSetLayoutUniform);
		descriptorSet->AddUniformBuffer(cameraUniformBuffer);
		descriptorSet->AddUniformBuffer(worldUniformBuffer);
		descriptorSet->Complete();
	}
	void VK::CreateGraphicsPipeline()
	{
		graphicsPipeline = new Pipeline(device, "assets/shaders/vert.spv", "assets/shaders/frag.spv");
		graphicsPipeline->SetRenderPass(renderPass);
		VkDescriptorSetLayout descriptorSetLayouts[]{ descriptorSetLayoutUniform, descriptorSetLayoutSampler };
		graphicsPipeline->SetDescriptorSetLayouts(descriptorSetLayouts, (uint32_t)std::size(descriptorSetLayouts));
		graphicsPipeline->Complete();
	}

	void VK::CreateFramebuffers()
	{
		swapChainFramebuffers.resize(swapChainImageViews.size());
	
		VkResult res;
		for (size_t i = 0; i < swapChainImageViews.size(); i++) 
		{
			VkImageView attachments[] = { swapChainImageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			res = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
			if(res != VK_SUCCESS)
				throw std::runtime_error("failed to create framebuffer!");
		}
	}
	void VK::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

		VkResult res = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create command pool!");
	}
	void VK::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

		VkResult res = vkAllocateCommandBuffers(device, &allocInfo, commandBuffer);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to allocate command buffers!");
	}

	void VK::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkResult res;
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]);
			if (res != VK_SUCCESS)
				throw std::exception("Failed to create semaphore!");
			
			res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore[i]);
			if (res != VK_SUCCESS)
				throw std::exception("Failed to create semaphore!");

			res = vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence[i]);
			if (res != VK_SUCCESS)
				throw std::exception("Failed to create fence!");
		}		
	}

	void VK::initializeImGui()
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		VkResult res = vkCreateDescriptorPool(VK::GetLogicalDevice(), &pool_info, nullptr, &imguiPool);
		if (res != VK_SUCCESS)
			throw std::exception("Couldn't create descriptor pool!");

		ImGui_ImplGlfw_InitForVulkan(App::GetWindow()->GetGLFWWindow(), true);
		
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = vkInstance;
		init_info.PhysicalDevice = physicalDevice;
		init_info.Device = device;
		init_info.Queue = graphicsQueue;
		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&init_info, renderPass);

		vkResetFences(device, 1, &inFlightFence[0]);
		vkResetCommandBuffer(commandBuffer[0], 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;


		res = vkBeginCommandBuffer(commandBuffer[0], &beginInfo);
		if (res != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");
		
		if (!ImGui_ImplVulkan_CreateFontsTexture(commandBuffer[0]))
			throw std::exception("failed to initialize ImGui!");

		res = vkEndCommandBuffer(commandBuffer[0]);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to record command buffer!");
		

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffer;

		res = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence[0]);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to submit draw command buffer!");

		vkWaitForFences(device, 1, &inFlightFence[0], VK_TRUE, UINT64_MAX);		

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
	void VK::terminateImGui()
	{
		vkDestroyDescriptorPool(device, imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	void VK::updateUniforms()
	{
		auto window = App::GetWindow();
		static glm::mat4 world(glm::rotate(glm::radians(60.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));// * glm::scale(glm::vec3(window->GetWindowHeight() * 0.5f, window->GetWindowHeight() * 0.5f, 1.0f)));

		const glm::mat4 cam_matrices[] = {
			glm::perspective(glm::radians(45.0f), (float)window->GetWindowWidth() / window->GetWindowHeight(), 0.1f, 1000.0f),
			//glm::ortho(-window->GetWindowWidth() * 0.5f, window->GetWindowWidth() * 0.5f, -window->GetWindowHeight() * 0.5f, window->GetWindowHeight() * 0.5f, 0.0f, 1.0f),
			glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		};

		cameraUniformBuffer->SendData(cam_matrices, sizeof(glm::mat4) * 2);
		worldUniformBuffer->SendData(&world, sizeof(glm::mat4));
		
		world *= glm::rotate(.01f, glm::vec3(0.0f, 0.0f, 1.0f));

	}
	void VK::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		VkResult res = vkBeginCommandBuffer(commandBuffer, &beginInfo);
		if (res != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());

		VkBuffer vertexBuffers[]{ vertexBuffer->GetVkBuffer() };
		VkDeviceSize offsets[]{ 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetVkBuffer(), offsets[0], VK_INDEX_TYPE_UINT32);

		VkDescriptorSet descSets[]{ descriptorSet->GetVkDescriptorSet()[currentFrame], crateTexture->GetDescriptorSet()};
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipelineLayout(), 0, (uint32_t)std::size(descSets), descSets, 0, nullptr);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDrawIndexed(commandBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer, VK_NULL_HANDLE);

		vkCmdEndRenderPass(commandBuffer);

		res = vkEndCommandBuffer(commandBuffer);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to record command buffer!");
	}
	void VK::drawFrame()
	{
		updateUniforms();

		vkWaitForFences(device, 1, &inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult res = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (res == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return;
		}
		else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
			throw std::exception("Failed to acquire swap chain image!");
		}

		vkResetFences(device, 1, &inFlightFence[currentFrame]);

		vkResetCommandBuffer(commandBuffer[currentFrame], 0);
		recordCommandBuffer(commandBuffer[currentFrame], imageIndex);
	
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer[currentFrame];
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		res = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence[currentFrame]);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to submit draw command buffer!");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		res = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
			RecreateSwapChain();
		else if (res != VK_SUCCESS)
			throw std::exception("Failed to present swap chain image!");

		++currentFrame %= MAX_FRAMES_IN_FLIGHT;
	}
	void VK::deviceWaitIdle()
	{
		vkDeviceWaitIdle(device);
	}

	void VK::CleanupVulkan()
	{
		delete vertexBuffer;
		delete indexBuffer;

		delete crateTexture;

		vkDestroyDescriptorSetLayout(device, descriptorSetLayoutUniform, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorSetLayoutSampler, nullptr);

		delete cameraUniformBuffer;
		delete worldUniformBuffer;
		delete descriptorSet;

		vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device, imageAvailableSemaphore[i], nullptr);
			vkDestroySemaphore(device, renderFinishedSemaphore[i], nullptr);
			vkDestroyFence(device, inFlightFence[i], nullptr);
		}

		vkDestroyCommandPool(device, commandPool, nullptr);

		delete graphicsPipeline;
		vkDestroyRenderPass(device, renderPass, nullptr);

		CleanupSwapChain();
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(vkInstance, surface, nullptr);

		if (ENABLE_VALIDATION_LAYERS)
			DestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);

		vkDestroyInstance(vkInstance, nullptr);
	}

	VkCommandBuffer VK::Helper::BeginOneTimeCommand()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = VK::GetCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(VK::GetLogicalDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}
	void VK::Helper::EndOneTimeCommand(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkQueue queue = VK::GetTransferQueue();
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkFreeCommandBuffers(VK::GetLogicalDevice(), VK::GetCommandPool(), 1, &commandBuffer);
	}
}