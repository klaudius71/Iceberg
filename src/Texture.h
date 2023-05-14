#ifndef _TEXTURE
#define _TEXTURE

#include "IcebergCore.h"

namespace Iceberg {

	class DescriptorSet;
	
	class Texture
	{
	public:
		Texture(const char* const filename, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		Texture(uint32_t width, uint32_t height);
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		~Texture();

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		VkImage GetImage() const;
		VkImageView GetImageView() const;
		VkSampler GetSampler() const;
		VkDescriptorSet GetDescriptorSet() const;
		VkDescriptorSet GetDescriptorSet(uint32_t index) const;

		void Resize(uint32_t width, uint32_t height);

		void Bind() const;

		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	private:
		void createImage(uint32_t width, uint32_t height, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void createImageView();
		void createSampler(VkFilter minFilter = VK_FILTER_LINEAR, VkFilter magFilter = VK_FILTER_LINEAR);
		void cleanup();

		VkDevice device;
		uint32_t width;
		uint32_t height;		
		VkImage image;
		VkDeviceMemory imageMemory;
		VkImageView imageView;
		VkSampler sampler;
		DescriptorSet* descriptorSet;
	};

}


#endif