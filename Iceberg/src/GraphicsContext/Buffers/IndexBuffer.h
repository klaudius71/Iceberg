#ifndef _INDEX_BUFFER
#define _INDEX_BUFFER

#include "Buffer.h"

namespace Iceberg {

	class IndexBuffer : public Buffer
	{
	public:
		IndexBuffer(VkDevice device, uint32_t numIndices);
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;
		~IndexBuffer() = default;
	};

}

#endif 