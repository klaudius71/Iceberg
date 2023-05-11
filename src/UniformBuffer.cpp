#include "ipch.h"
#include "UniformBuffer.h"
#include "VK.h"

namespace Iceberg {

	UniformBuffer::UniformBuffer(VkDevice device, VkDeviceSize size, const uint32_t bind, VkShaderStageFlags stage_flags)
		: buffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
		descriptorSetLayout(nullptr),
		dataPtr(nullptr)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding {};
		uboLayoutBinding.binding = bind;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = stage_flags;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VkResult res = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create descriptor set layout!");

		buffer.Bind();
		buffer.Map(dataPtr);
	}
	UniformBuffer::~UniformBuffer()
	{
		vkDestroyDescriptorSetLayout(buffer.GetDevice(), descriptorSetLayout, nullptr);
	}

	VkDescriptorSetLayout UniformBuffer::GetDescriptorSetLayout() const
	{
		return descriptorSetLayout;
	}

	VkBuffer UniformBuffer::GetBuffer() const
	{
		return buffer.GetVkBuffer();
	}

	void* UniformBuffer::GetDataPointer() const
	{
		return dataPtr;
	}

}
