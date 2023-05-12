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
		void* GetDataPointer() const;

	private:
		Buffer buffer;
		void* dataPtr;
	};

}

#endif