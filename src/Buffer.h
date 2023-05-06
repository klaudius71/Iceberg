#ifndef _BUFFER
#define _BUFFER

namespace Iceberg {

	class Buffer
	{
	protected:
		Buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags properties);
		Buffer() = delete;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		virtual ~Buffer();

	public:
		operator VkBuffer() const;

		VkResult Bind() const;

		VkResult Map(void*& mem) const;
		void Unmap() const;

	private:
		VkDevice dev;
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		uint64_t size;
	};

}

#endif