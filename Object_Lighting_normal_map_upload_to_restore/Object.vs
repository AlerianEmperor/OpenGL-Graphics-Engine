#version 430 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 o_texcoord;
out vec3 o_normal;

void main()
{
	o_texcoord = i_texcoord;

	o_normal = (transpose(inverse(u_model)) * vec4(i_normal, 1.0f)).xyz;

	gl_Position = u_projection * u_view * u_model * vec4(i_pos, 1.0f);
}
