#ifndef _IMAGE
#define _IMAGE

namespace Iceberg {

	class Texture;
	class StagingBuffer;

	class Image
	{
	public:
		Image(uint32_t width = 512, uint32_t height = 512);
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
		~Image();

		void Resize(uint32_t width, uint32_t height);
		void SetData(const uint32_t* pixels, uint32_t size);

		ImTextureID GetImGuiTexture() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		Texture* vkTexture;
		StagingBuffer* stagingBuffer;
		uint32_t width;
		uint32_t height;
	};
}

#endif