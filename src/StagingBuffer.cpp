#include "ipch.h"
#include "StagingBuffer.h"
#include "VK.h"

namespace Iceberg {

	StagingBuffer::StagingBuffer(VkDevice device, VkDeviceSize size)
		: Buffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	{
		cpuWriteable = true;
	}

	void StagingBuffer::Resize(VkDeviceSize size)
	{
		vkDestroyBuffer(dev, buffer, nullptr);
		vkFreeMemory(dev, bufferMemory, nullptr);

		this->size = size;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult res = vkCreateBuffer(dev, &bufferInfo, nullptr, &buffer);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(dev, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VK::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		res = vkAllocateMemory(dev, &allocInfo, nullptr, &bufferMemory);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to allocate buffer memory!");
	}

	void StagingBuffer::TransferBuffer(const Buffer* const buf) const
	{
		assert(buf);
		buf->Bind();

		VkCommandPool commandPool = VK::GetCommandPool();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		VkResult res = vkAllocateCommandBuffers(dev, &allocInfo, &commandBuffer);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to allocate command buffer!");

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		res = vkBeginCommandBuffer(commandBuffer, &beginInfo);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to begin recording command buffer!");

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, buffer, *buf, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkQueue transferQueue = VK::GetTransferQueue();
		vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(transferQueue);

		vkFreeCommandBuffers(dev, commandPool, 1, &commandBuffer);
	}

}
