#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 uv_coord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main()
{
	outColor = texture(texSampler, uv_coord);
}