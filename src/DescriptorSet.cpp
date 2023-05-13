#include "ipch.h"
#include "DescriptorSet.h"
#include "VK.h"
#include "UniformBuffer.h"
#include "Texture.h"

namespace Iceberg {

	DescriptorSet::DescriptorSet(VkDescriptorSetLayout layout)
		: objects(new std::vector<void*>()), 
		descriptorWrite(new std::vector<VkWriteDescriptorSet>()), 
		complete(false)
	{
		assert(layout != VK_NULL_HANDLE);
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = VK::GetDescriptorPool();
		allocInfo.descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = layouts.data();

		VkResult res;
		res = vkAllocateDescriptorSets(VK::GetLogicalDevice(), &allocInfo, descriptorSet);
		if (res != VK_SUCCESS)
			throw std::exception("Failed to allocate descriptor set!");
	}
	DescriptorSet::~DescriptorSet()
	{
		delete descriptorWrite;
	}

	void DescriptorSet::AddUniformBuffer(UniformBuffer* buf)
	{
		assert(!complete);
		objects->emplace_back(buf);
		descriptorWrite->emplace_back(VkWriteDescriptorSet{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			VK_NULL_HANDLE,
			(uint32_t)descriptorWrite->size(),
			0,
			1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			nullptr,
			nullptr,
			nullptr
		});
	}
	void DescriptorSet::AddTexture(Texture* tex)
	{
		assert(!complete);
		objects->emplace_back(tex);
		descriptorWrite->emplace_back(VkWriteDescriptorSet{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			VK_NULL_HANDLE,
			(uint32_t)descriptorWrite->size(),
			0,
			1,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			nullptr,
			nullptr,
			nullptr
		});
	}

	// This is so scuffed
	void DescriptorSet::Complete()
	{
		assert(!complete);
		assert(descriptorWrite->size() != 0);

		// Create the descriptor infos
		std::vector<void*> bufferInfos;
		for (int i = 0; i < descriptorWrite->size(); i++)
		{
			switch ((*descriptorWrite)[i].descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			{
				VkDescriptorBufferInfo* bufferInfo = (VkDescriptorBufferInfo*)bufferInfos.emplace_back(new VkDescriptorBufferInfo);
				(*descriptorWrite)[i].pBufferInfo = bufferInfo;
				break;
			}
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			{
				VkDescriptorImageInfo* imageInfo = (VkDescriptorImageInfo*)bufferInfos.emplace_back(new VkDescriptorImageInfo);
				(*descriptorWrite)[i].pImageInfo = imageInfo;
				break;
			}
			default:
				assert(false);
			}
		}

		// For each frame in flight
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			// Populate each buffer info
			for (int j = 0; j < descriptorWrite->size(); j++)
			{
				(*descriptorWrite)[j].dstSet = descriptorSet[i];
				switch ((*descriptorWrite)[j].descriptorType)
				{
				case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				{
					VkDescriptorBufferInfo* bufferInfo = (VkDescriptorBufferInfo*)bufferInfos[j];
					UniformBuffer* uniformBuffer = (UniformBuffer*)(*objects)[j];
					bufferInfo->buffer = uniformBuffer->GetBuffer((uint32_t)i);
					bufferInfo->offset = 0;
					bufferInfo->range = uniformBuffer->GetBufferSize();
					break;
				}
				case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				{
					VkDescriptorImageInfo* imageInfo = (VkDescriptorImageInfo*)bufferInfos[j];
					Texture* texture = (Texture*)(*objects)[j];
					imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo->imageView = texture->GetImageView();
					imageInfo->sampler = texture->GetSampler();
					break;
				}
				default:
					assert(false);
				}
			}

			vkUpdateDescriptorSets(VK::GetLogicalDevice(), (uint32_t)descriptorWrite->size(), descriptorWrite->data(), 0, nullptr);
		}

		for (auto bufferInfo : bufferInfos)
			delete bufferInfo;
		
		delete descriptorWrite;
		descriptorWrite = nullptr;

		delete objects;
		objects = nullptr;

		complete = true;
	}

	VkDescriptorSet* DescriptorSet::GetVkDescriptorSet()
	{
		if (!complete)
			printf("Getting an incomplete descriptor set!\n");

		return descriptorSet;
	}

}