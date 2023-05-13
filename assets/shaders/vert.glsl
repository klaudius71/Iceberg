#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 col;
layout(location = 2) in vec2 uv;

layout(set = 0, binding = 0) uniform CameraData 
{
	mat4 proj_matrix;
	mat4 view_matrix;
} cam_matrices;

layout(set = 0, binding = 1) uniform WorldData
{
	mat4 world_matrix;
} world;

layout(location = 0) out vec3 color;
layout(location = 1) out vec2 uv_coord;

void main()
{
	gl_Position = cam_matrices.proj_matrix * cam_matrices.view_matrix * world.world_matrix * vec4(position, 1.0);
	color = col;
	uv_coord = uv;
}