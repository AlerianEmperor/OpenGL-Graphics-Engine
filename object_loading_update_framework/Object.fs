#version 430 core

in vec2 o_texcoord;
in vec3 o_normal;

uniform sampler2D diffuse_texture;

out vec4 fragColor;

void main()
{
	fragColor = texture(diffuse_texture, o_texcoord);
}
