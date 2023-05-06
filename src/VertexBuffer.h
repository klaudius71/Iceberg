#ifndef _VERTEX_BUFFER
#define _VERTEX_BUFFER

#include "Buffer.h"
#include "Vertex.h"

namespace Iceberg {

	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer(VkDevice device, const std::vector<Vertex>& vertices);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		~VertexBuffer() = default;
	};

}

#endif 