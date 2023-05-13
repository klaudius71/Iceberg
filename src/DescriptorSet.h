#ifndef _DESCRIPTOR_SET
#define _DESCRIPTOR_SET

#include "IcebergCore.h"

namespace Iceberg {

	class UniformBuffer;
	class Texture;

	class DescriptorSet
	{
	public:
		DescriptorSet(VkDescriptorSetLayout layout);
		DescriptorSet(const DescriptorSet&) = delete;
		DescriptorSet& operator=(const DescriptorSet&) = delete;
		~DescriptorSet();

		void AddUniformBuffer(UniformBuffer* buf);
		void AddTexture(Texture* tex);

		void Complete();

		VkDescriptorSet* GetVkDescriptorSet();

	private:
		std::vector<void*>* objects;
		std::vector<VkWriteDescriptorSet>* descriptorWrite;

		VkDescriptorSet descriptorSet[MAX_FRAMES_IN_FLIGHT];
		bool complete;
	};

}


#endif