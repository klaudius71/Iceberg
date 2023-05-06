#include "ipch.h"
#include "VertexBuffer.h"

#include "VK.h"

namespace Iceberg {

	VertexBuffer::VertexBuffer(VkDevice device, const std::vector<Vertex>& vertices)
		: Buffer(device, 
			sizeof(Vertex) * vertices.size(), 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	{
		this->Bind();

		void* mem;
		this->Map(mem);
		memcpy_s(mem, sizeof(Vertex) * vertices.size(), vertices.data(), sizeof(Vertex) * vertices.size());
		this->Unmap();
	}
}
