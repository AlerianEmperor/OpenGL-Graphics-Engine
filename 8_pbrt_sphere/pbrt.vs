#version 430 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

out vec2 texcoord;
out vec3 worldPos;
out vec3 normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
	texcoord = i_texcoord;
	worldPos = (u_model * vec4(i_pos, 1.0f)).xyz;
	normal = i_normal;

	gl_Position = u_projection * u_view * u_model * vec4(i_pos, 1.0f);
}