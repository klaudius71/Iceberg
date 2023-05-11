#ifndef _UNIFORM_BUFFER
#define _UNIFROM_BUFFER

#include "Buffer.h"

namespace Iceberg {

	class UniformBuffer
	{
	public:
		UniformBuffer(VkDevice device, VkDeviceSize size, const uint32_t bind, VkShaderStageFlags stage_flags = VK_SHADER_STAGE_ALL_GRAPHICS);
		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer& operator=(const UniformBuffer&) = delete;
		~UniformBuffer();

		VkDescriptorSetLayout GetDescriptorSetLayout() const;

		void* GetDataPointer() const;

	private:
		Buffer buffer;
		VkDescriptorSetLayout descriptorSetLayout;
		void* dataPtr;
	};

}

#endif