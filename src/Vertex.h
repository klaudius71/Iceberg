#ifndef _VERTEX
#define _VERTEX

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 col;

	static constexpr VkVertexInputBindingDescription GetBindingDescription()
	{
		return { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX };
	}
	static constexpr std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
	{
		return
		{ {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT,  0 },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, 16 }
		} };
	}
};

#endif