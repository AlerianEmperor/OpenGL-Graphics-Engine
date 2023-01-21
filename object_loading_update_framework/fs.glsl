#version 430

in vec2 tex;

in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;

in vec3 varyingHalfVector;

out vec4 frag_color;

uniform vec4 Kd;
uniform float Ns;
uniform sampler2D DiffuseTexture;
uniform sampler2D MaskTexture;
uniform bool useTexture;
uniform bool useMask;

uniform vec4 globalAmbient;

struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

uniform Light light;


void main()
{
	vec4 color;

	if(useTexture)
	{
		vec4 a = texture(MaskTexture, tex);
		if(a.w < 0.5f)
			discard;
		else
			color = texture2D(DiffuseTexture, tex);
			//if(color.w<0.5f)
			//discard;
	}
	else
		color = Kd;
	
	vec3 L = normalize(varyingLightDir);
	vec3 N = normalize(varyingNormal);
	vec3 H = normalize(varyingHalfVector);

	frag_color = color * (light.ambient + light.diffuse * max(dot(L, N), 0.0f)
	+ light.specular * pow(max(dot(H, N) * 0.9f, 0.0f), 30.0f)); //pow(max(dot(H, N), 0.0f), 2000.0f);

	//frag_color = vec4(N, 1.0f);
	
}