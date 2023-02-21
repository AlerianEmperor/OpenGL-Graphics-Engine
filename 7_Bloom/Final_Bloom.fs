#version 430 core

out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

void main()
{
	const float gamma = 2.2f;
	
	vec3 hdrColor = texture(scene, texcoord).rgb;

	vec3 bloomColor = texture(bloomBlur, texcoord).rgb;

	if(bloom)
		hdrColor += bloomColor;

	vec3 result = vec3(1.0f) - exp(-hdrColor * exposure);

	result = pow(result, vec3(1.0f / gamma));

	FragColor = vec4(result, 1.0f);
}