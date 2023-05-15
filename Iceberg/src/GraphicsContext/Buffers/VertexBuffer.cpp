#include "ipch.h"
#include "VertexBuffer.h"

#include "VK.h"

namespace Iceberg {

	VertexBuffer::VertexBuffer(VkDevice device, VkDeviceSize size)
		: Buffer(device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		cpuWriteable = false;
	}
}
