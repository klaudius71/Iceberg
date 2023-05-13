#ifndef _TEXTURE
#define _TEXTURE

#include "IcebergCore.h"

namespace Iceberg {
	
	class Texture
	{
	public:
		Texture(const char* const filename, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		~Texture();

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		VkImage GetImage() const;
		VkImageView GetImageView() const;
		VkSampler GetSampler() const;
		VkDescriptorSet GetDescriptorSet() const;

		void Bind() const;

	private:
		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		VkDevice device;
		uint32_t width;
		uint32_t height;		
		VkImage image;
		VkDeviceMemory imageMemory;
		VkImageView imageView;
		VkSampler sampler;
		VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
	};

}


#endif