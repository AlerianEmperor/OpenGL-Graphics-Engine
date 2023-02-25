#version 430 core

layout (location = 0) in vec3 i_pos;

out vec3 worldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	worldPos = i_pos;
	gl_Position = projection * view * vec4(worldPos, 1.0f);
}