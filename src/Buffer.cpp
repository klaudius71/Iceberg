#include "ipch.h"
#include "Buffer.h"
#include "VK.h"

namespace Iceberg {

	Buffer::Buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags properties)
		: dev(device), size(size)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = bufferUsage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult res = vkCreateBuffer(dev, &bufferInfo, nullptr, &buffer);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(dev, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VK::FindMemoryType(memRequirements.memoryTypeBits, properties);

		res = vkAllocateMemory(dev, &allocInfo, nullptr, &bufferMemory);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to allocate buffer memory!");
	}
	Buffer::~Buffer()
	{
		vkDestroyBuffer(dev, buffer, nullptr);
		vkFreeMemory(dev, bufferMemory, nullptr);
	}

	VkDevice Buffer::GetDevice() const
	{
		return dev;
	}

	VkBuffer Buffer::GetVkBuffer() const
	{
		return buffer;
	}
	Buffer::operator VkBuffer() const
	{
		return buffer;
	}

	bool Buffer::IsCPUWriteable() const
	{
		return cpuWriteable;
	}

	VkResult Buffer::Bind() const
	{
		return vkBindBufferMemory(dev, buffer, bufferMemory, 0);
	}

	VkResult Buffer::Map(void*& mem) const
	{
		assert(cpuWriteable);
		return vkMapMemory(dev, bufferMemory, 0, size, 0, &mem);
	}
	void Buffer::Unmap() const
	{
		assert(cpuWriteable);
		vkUnmapMemory(dev, bufferMemory);
	}

}
