#include "ipch.h"
#include "Texture.h"
#include "StagingBuffer.h"
#include "VK.h"
#include "DescriptorSet.h"

namespace Iceberg {

	Texture::Texture(const char* const filename, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
		: device(VK::GetLogicalDevice())
	{
		assert(filename);

		int x, y, components;
		uint8_t* img = stbi_load(filename, &x, &y, &components, 4);
		if (!img)
			throw std::runtime_error(("Failed to load " + std::string(filename)).c_str());
		width = x;
		height = y;

		const VkDeviceSize deviceSize = x * y * components;
		StagingBuffer stagingBuffer(device, deviceSize);
		void* mem;
		stagingBuffer.Map(mem);
		memcpy(mem, img, deviceSize);
		stagingBuffer.Unmap();
		stbi_image_free(img);

		// Create image
		createImage(width, height, usage, properties);

		// Transition the image layout and upload texture data
		vkBindImageMemory(device, image, imageMemory, 0);
		stagingBuffer.TransferBuffer(this);
	
		// Create image view and sampler
		createImageView();
		createSampler();

		// Descriptor set
		descriptorSet = new DescriptorSet(VK::GetDescriptorSetLayoutSampler());
		descriptorSet->AddTexture(this);
		descriptorSet->Complete();
	}
	Texture::Texture(uint32_t width, uint32_t height)
		: device(VK::GetLogicalDevice()), width(width), height(height)
	{
		// Create image
		createImage(width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkBindImageMemory(device, image, imageMemory, 0);

		//transitionImageLayout(imageInfo.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		//stagingBuffer.TransferBuffer(this);
		//transitionImageLayout(imageInfo.format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// Create image view and sampler
		createImageView();
		createSampler(VK_FILTER_NEAREST, VK_FILTER_NEAREST);

		// Descriptor set
		descriptorSet = new DescriptorSet(VK::GetDescriptorSetLayoutSampler());
		descriptorSet->AddTexture(this);
		descriptorSet->Complete();
	}
	Texture::~Texture()
	{
		cleanup();
		vkDestroySampler(device, sampler, nullptr);
	}

	uint32_t Texture::GetWidth() const
	{
		return width;
	}
	uint32_t Texture::GetHeight() const
	{
		return height;
	}
	VkImage Texture::GetImage() const
	{
		return image;
	}
	VkImageView Texture::GetImageView() const
	{
		return imageView;
	}
	VkSampler Texture::GetSampler() const
	{
		return sampler;
	}
	VkDescriptorSet Texture::GetDescriptorSet() const
	{
		return descriptorSet->GetVkDescriptorSet()[VK::GetCurrentFrame()];
	}
	VkDescriptorSet Texture::GetDescriptorSet(uint32_t index) const
	{
		assert(index >= 0 && index < MAX_FRAMES_IN_FLIGHT);
		return descriptorSet->GetVkDescriptorSet()[index];
	}

	void Texture::Resize(uint32_t _width, uint32_t _height)
	{
		cleanup();
		width = _width;
		height = _height;

		// Create image
		createImage(width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vkBindImageMemory(device, image, imageMemory, 0);

		// Create image view
		createImageView();

		// Descriptor set
		descriptorSet = new DescriptorSet(VK::GetDescriptorSetLayoutSampler());
		descriptorSet->AddTexture(this);
		descriptorSet->Complete();
	}

	void Texture::Bind() const
	{
		vkBindImageMemory(device, image, imageMemory, 0);
	}

	void Texture::createImage(uint32_t _width, uint32_t _height, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = _width;
		imageInfo.extent.height = _height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		VkResult res;
		res = vkCreateImage(device, &imageInfo, nullptr, &image);
		if (res != VK_SUCCESS)
			throw std::runtime_error("Failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VK::FindMemoryType(memRequirements.memoryTypeBits, properties);

		res = vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory);
		if (res != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate image memory!");
	}
	void Texture::transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = VK::Helper::BeginOneTimeCommand();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags srcStage = 0;
		VkPipelineStageFlags dstStage = 0;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		VK::Helper::EndOneTimeCommand(commandBuffer);
	}
	void Texture::createImageView()
	{
		// Image view
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkResult res = vkCreateImageView(device, &viewInfo, nullptr, &imageView);
		if (res != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture image view!");
	}
	void Texture::createSampler(VkFilter minFilter, VkFilter magFilter)
	{
		// Sampler
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.minFilter = minFilter;
		samplerInfo.magFilter = magFilter;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16.0f; // Query for this
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VkResult res = vkCreateSampler(device, &samplerInfo, nullptr, &sampler);
		if (res != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture sampler!");
	}
	void Texture::cleanup()
	{
		delete descriptorSet;
		descriptorSet = nullptr;
		vkDestroyImageView(device, imageView, nullptr);
		vkDestroyImage(device, image, nullptr);
		vkFreeMemory(device, imageMemory, nullptr);
	}
}
