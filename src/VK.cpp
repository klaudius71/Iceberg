#include "ipch.h"
#include "VK.h"
#include "App.h"
#include "Window.h"

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
		instance = new VK;
	}
	VkInstance VK::GetVkInstance()
	{
		return Instance().vkInstance;
	}
	void VK::Terminate()
	{
		assert(instance && "VK instance not created!");
		delete instance;
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
		CreateGraphicsPipeline();
	}
	VK::~VK()
	{
		CleanupVulkan();
	}


	VKAPI_ATTR VkBool32 VKAPI_CALL VK::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
	{
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			printf("validation layer: %s\n", pCallbackData->pMessage);

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
	std::vector<uint8_t> VK::ReadBinaryFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if(!file.is_open())
			throw std::exception(("Could not open file " + filename).c_str());

		size_t file_size = (size_t)file.tellg();
		std::vector<uint8_t> buffer(file_size);
		file.seekg(0);
		file.read((char*)buffer.data(), file_size);
		file.close();
		return buffer;
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

		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		return availableFormats[0];
	}
	VkPresentModeKHR VK::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
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
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
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
			&& swapChainAdequate;
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

	VkShaderModule VK::CreateShaderModule(const std::vector<uint8_t>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = (uint32_t*)code.data();

		VkShaderModule shaderModule;
		VkResult res = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create shader module!");

		return shaderModule;
	}
	void VK::CreateGraphicsPipeline()
	{
		auto vertShaderCode = ReadBinaryFile("assets/shaders/vert.spv");
		auto fragShaderCode = ReadBinaryFile("assets/shaders/frag.spv");

		VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
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
	void VK::CleanupVulkan()
	{
		for (auto imageView : swapChainImageViews)
			vkDestroyImageView(device, imageView, nullptr);

		vkDestroySwapchainKHR(device, swapChain, nullptr);
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(vkInstance, surface, nullptr);

		if (ENABLE_VALIDATION_LAYERS)
			DestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);

		vkDestroyInstance(vkInstance, nullptr);
	}

}

