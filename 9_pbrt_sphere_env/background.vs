#version 430 core

layout (location = 0) in vec3 i_pos;

uniform mat4 projection;
uniform mat4 view;

out vec3 worldPos;

void main()
{
	worldPos = i_pos;

	//remove translation
	mat4 rotView = mat4(mat3(view));

	vec4 clipPos = projection * rotView * vec4(worldPos, 1.0f);

	gl_Position = clipPos.xyww;
}