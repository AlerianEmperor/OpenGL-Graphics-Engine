#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Diffuse_Texture;
uniform sampler2D Diffuse_Mask;
uniform vec4 Kd;
uniform bool useTexture;
uniform bool useMask;

void main()
{ 
	if(useTexture)   
	{
		if(useMask)
		{
			vec4 a = texture(Diffuse_Mask, TexCoords);
			if(a.w < 0.5f)
				discard;
			else
				FragColor = texture(Diffuse_Texture, TexCoords);
		}
		else
			FragColor = texture(Diffuse_Texture, TexCoords);
	}
	else
		FragColor = Kd;
}