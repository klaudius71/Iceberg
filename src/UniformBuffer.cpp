#include "ipch.h"
#include "UniformBuffer.h"
#include "VK.h"

namespace Iceberg {

	UniformBuffer::UniformBuffer(VkDevice device, VkDeviceSize size)
		: buffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
		bufferSize(size),
		dataPtr(nullptr)
	{
		buffer.Bind();
		buffer.Map(dataPtr);
	}
	UniformBuffer::~UniformBuffer()
	{
	}

	VkBuffer UniformBuffer::GetBuffer() const
	{
		return buffer.GetVkBuffer();
	}

	void UniformBuffer::SendData(const void* data, uint64_t size) const
	{
		assert(data);
		assert(size <= bufferSize);
		memcpy_s(dataPtr, bufferSize, data, size);
	}

}
