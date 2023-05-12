#ifndef _PIPELINE
#define _PIPELINE

// Pretty hardcoded for now

namespace Iceberg {
	
	class Pipeline
	{
	public:
		Pipeline(VkDevice device, const char* const vertex_shader_filename, const char* const fragment_shader_filename);
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		~Pipeline();

		void SetRenderPass(VkRenderPass renderPass);
		void SetDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);

		void Complete();

		VkPipelineLayout GetPipelineLayout() const;
		VkPipeline GetPipeline() const;

	private:
		static std::vector<uint8_t> ReadBinaryFile(const char* const filename);
		VkShaderModule CreateShaderModule(const std::vector<uint8_t>& code);

		VkDevice device;
		const char* const vertex_shader_filename;
		const char* const fragment_shader_filename;
		VkDescriptorSetLayout descriptorSetLayout;
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
	};

}

#endif