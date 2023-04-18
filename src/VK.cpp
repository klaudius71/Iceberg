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

		QueueFamilyIndices queueFamilies = FindQueueFamilies(dev);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
			&& deviceFeatures.geometryShader
			&& queueFamilies.graphicsFamilyExists
			&& queueFamilies.computeFamilyExists
			&& CheckDeviceExtensionSupport(dev);
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
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(vkInstance, surface, nullptr);

		if (ENABLE_VALIDATION_LAYERS)
			DestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);

		vkDestroyInstance(vkInstance, nullptr);
	}

}

