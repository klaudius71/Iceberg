#include "ipch.h"
#include "VertexBuffer.h"
#include "VK.h"

namespace Iceberg {

	VertexBuffer::VertexBuffer(VkDevice device, const std::vector<Vertex>& vertices)
		: dev(device)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(Vertex) * vertices.size();
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult res = vkCreateBuffer(dev, &bufferInfo, nullptr, &vertexBuffer);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create vertex buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(dev, vertexBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VK::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		res = vkAllocateMemory(dev, &allocInfo, nullptr, &vertexBufferMemory);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to allocate vertex buffer memory!");

		vkBindBufferMemory(dev, vertexBuffer, vertexBufferMemory, 0);
		void* data;
		vkMapMemory(dev, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy_s(data, bufferInfo.size, vertices.data(), sizeof(Vertex) * vertices.size());
		vkUnmapMemory(dev, vertexBufferMemory);
	}
	VertexBuffer::~VertexBuffer()
	{
		vkDestroyBuffer(dev, vertexBuffer, nullptr);
		vkFreeMemory(dev, vertexBufferMemory, nullptr);
	}

	VertexBuffer::operator VkBuffer() const
	{
		return vertexBuffer;
	}
}
