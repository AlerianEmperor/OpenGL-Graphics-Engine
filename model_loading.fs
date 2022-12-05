#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Diffuse_Texture;
uniform sampler2D Diffuse_Mask;
uniform vec4 Kd;
uniform bool useTexture;

void main()
{ 
	if(useTexture)   
		FragColor = texture(Diffuse_Texture, TexCoords);
	else
		FragColor = Kd;
}