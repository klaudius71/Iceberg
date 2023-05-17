#include "ipch.h"
#include "UniformBuffer.h"
#include "Buffer.h"
#include "VK.h"

namespace Iceberg {

	UniformBuffer::UniformBuffer(VkDevice device, VkDeviceSize size)
		: buffer((Buffer*)malloc(sizeof(Buffer)* MAX_FRAMES_IN_FLIGHT)),
		bufferSize(size), dataPtr{}
	{
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			new(&buffer[i]) Buffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			buffer[i].Bind();
			buffer[i].Map(dataPtr[i]);
		}
	}
	UniformBuffer::~UniformBuffer()
	{
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			buffer[i].~Buffer();

		free(buffer);
	}

	VkBuffer UniformBuffer::GetBuffer(uint32_t index) const
	{
		assert(index >= 0 && index < MAX_FRAMES_IN_FLIGHT);
		return buffer[index].GetVkBuffer();
	}
	uint64_t UniformBuffer::GetBufferSize() const
	{
		return bufferSize;
	}

	void UniformBuffer::SendData(const void* data, uint64_t size) const
	{
		assert(data);
		assert(size <= bufferSize);
		memcpy_s(dataPtr[VK::GetCurrentFrame()], bufferSize, data, size);
	}

}
