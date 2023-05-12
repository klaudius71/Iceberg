#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 uv_coord;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(uv_coord, 0.0, 1.0);
}