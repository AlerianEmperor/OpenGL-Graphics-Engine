#version 430 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_texcoord;

out vec2 texcoord;

void main()
{
	texcoord = i_texcoord;
	gl_Position = vec4(i_pos, 1.0f);
}