#include "ipch.h"
#include "Image.h"
#include "Texture.h"
#include "StagingBuffer.h"
#include "VK.h"

namespace Iceberg {

	Image::Image(uint32_t width, uint32_t height)
		: width(width), height(height), vkTexture(new Texture(width, height)), stagingBuffer(new StagingBuffer(VK::GetLogicalDevice(), width * height * sizeof(uint32_t)))
	{
	}
	Image::~Image()
	{
		delete stagingBuffer;
		delete vkTexture;
	}

	void Image::Resize(uint32_t _width, uint32_t _height)
	{
		if (width == _width && height == _height)
			return;

		// Can do better than this.
		// Probably send this Image's Vulkan handles to a queue that takes care of releasing
		VK::DeviceWaitIdle();

		width = _width;
		height = _height;
		stagingBuffer->Resize(_width * _height * 4);
		vkTexture->Resize(_width, _height);
	}
	void Image::SetData(const uint32_t* pixels, uint32_t size)
	{
		assert(pixels);
		assert(size <= (width * height * sizeof(uint32_t)));
	
		void* mem;
		stagingBuffer->Map(mem);
		memcpy_s(mem, width * height * 4, pixels, width * height * 4);
		stagingBuffer->Unmap();
		stagingBuffer->TransferBuffer(vkTexture);
	}

	ImTextureID Image::GetImGuiTexture() const
	{
		return vkTexture->GetDescriptorSet();
	}
	uint32_t Image::GetWidth() const
	{
		return width;
	}
	uint32_t Image::GetHeight() const
	{
		return height;
	}

}
