#ifndef _VERTEX
#define _VERTEX

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec2 uv;

	static constexpr VkVertexInputBindingDescription GetBindingDescription()
	{
		return { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX };
	}
	static constexpr std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
	{
		return
		{ {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, col) },
			{ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) }
		} };
	}
};

#endif