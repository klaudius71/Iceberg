#ifndef _VERTEX_BUFFER
#define _VERTEX_BUFFER

#include "Buffer.h"

namespace Iceberg {

	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer(VkDevice device, VkDeviceSize size);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		~VertexBuffer() = default;
	};

}

#endif 