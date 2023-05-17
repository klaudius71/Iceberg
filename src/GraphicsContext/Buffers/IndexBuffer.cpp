#include "ipch.h"
#include "IndexBuffer.h"

namespace Iceberg {

	IndexBuffer::IndexBuffer(VkDevice device, uint32_t numIndices)
		: Buffer(device, numIndices * sizeof(uint32_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		cpuWriteable = false;
	}

}
