#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 col;

layout(binding = 0) uniform Data
{
	mat4 proj_matrix;
	mat4 view_matrix;
	mat4 world_matrix;
} matrices;

layout(location = 0) out vec3 color;

void main()
{
	gl_Position = matrices.proj_matrix * matrices.view_matrix * matrices.world_matrix * vec4(position, 1.0);
	color = col;
}