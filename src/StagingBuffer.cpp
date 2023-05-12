#include "ipch.h"
#include "StagingBuffer.h"
#include "VK.h"
#include "Texture.h"

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

		VkCommandBuffer commandBuffer = VK::Helper::BeginOneTimeCommand();

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, buffer, *buf, 1, &copyRegion);

		VK::Helper::EndOneTimeCommand(commandBuffer);
	}
	void StagingBuffer::TransferBuffer(const Texture* const buf) const
	{
		assert(buf);
		//this->Bind();
		buf->Bind();
	
		VkCommandBuffer commandBuffer = VK::Helper::BeginOneTimeCommand();
	
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = 
		{
			buf->GetWidth(),
			buf->GetHeight(),
			1
		};
	
		vkCmdCopyBufferToImage(commandBuffer, buffer, buf->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	
		VK::Helper::EndOneTimeCommand(commandBuffer);
	}

}
