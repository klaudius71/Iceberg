#ifndef _UNIFORM_BUFFER
#define _UNIFROM_BUFFER

#include "Buffer.h"
#include "IcebergCore.h"

namespace Iceberg {

	class UniformBuffer
	{
	public:
		UniformBuffer(VkDevice device, VkDeviceSize size);
		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer& operator=(const UniformBuffer&) = delete;
		~UniformBuffer() = default;

		VkBuffer GetBuffer(uint32_t index) const;
		uint64_t GetBufferSize() const;

		void SendData(const void* data, uint64_t size) const;

	private:
		Buffer buffer[MAX_FRAMES_IN_FLIGHT];
		uint64_t bufferSize;
		void* dataPtr[MAX_FRAMES_IN_FLIGHT];
	};

}

#endif