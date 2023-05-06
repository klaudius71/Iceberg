#ifndef _STAGING_BUFFER
#define _STAGING_BUFFER

#include "Buffer.h"

namespace Iceberg {

	class StagingBuffer : public Buffer
	{
	public:
		StagingBuffer(VkDevice device, VkDeviceSize size);
		StagingBuffer(const StagingBuffer&) = delete;
		StagingBuffer& operator=(const StagingBuffer&) = delete;
		~StagingBuffer() = default;

		void Resize(VkDeviceSize size);

		void TransferBuffer(const Buffer* const buf) const;
	};

}

#endif