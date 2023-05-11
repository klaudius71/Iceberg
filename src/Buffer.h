#ifndef _BUFFER
#define _BUFFER

namespace Iceberg {

	class Buffer
	{
	public:
		Buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags properties);
		Buffer() = delete;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		virtual ~Buffer();

		VkDevice GetDevice() const;
		VkBuffer GetVkBuffer() const;
		operator VkBuffer() const;

		bool IsCPUWriteable() const;

		VkResult Bind() const;
		VkResult Map(void*& mem) const;
		void Unmap() const;

	protected:
		VkDevice dev;
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		uint64_t size;
		bool cpuWriteable;
	};

}

#endif