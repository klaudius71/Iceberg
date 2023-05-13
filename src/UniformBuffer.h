#ifndef _UNIFORM_BUFFER
#define _UNIFROM_BUFFER

#include "Buffer.h"

namespace Iceberg {

	class UniformBuffer
	{
	public:
		UniformBuffer(VkDevice device, VkDeviceSize size);
		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer& operator=(const UniformBuffer&) = delete;
		~UniformBuffer();

		VkBuffer GetBuffer() const;
		uint64_t GetBufferSize() const;

		void SendData(const void* data, uint64_t size) const;

	private:
		Buffer buffer;
		uint64_t bufferSize;
		void* dataPtr;
	};

}

#endif