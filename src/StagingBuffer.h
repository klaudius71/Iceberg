#ifndef _STAGING_BUFFER
#define _STAGING_BUFFER

#include "Buffer.h"

namespace Iceberg {

	class Texture;

	class StagingBuffer : public Buffer
	{
	public:
		StagingBuffer(VkDevice device, VkDeviceSize size);
		StagingBuffer(const StagingBuffer&) = delete;
		StagingBuffer& operator=(const StagingBuffer&) = delete;
		~StagingBuffer() = default;

		void Resize(VkDeviceSize size);

		void TransferBuffer(const Buffer* const buf) const;
		void TransferBuffer(Texture* const buf) const;
	};

}

#endif