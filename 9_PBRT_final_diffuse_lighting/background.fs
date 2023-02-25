#version 430 core

out vec4 fragColor;
in vec3 worldPos;

uniform samplerCube enviromentMap;

void main()
{
	vec3 envColor = texture(enviromentMap, worldPos).rgb;

	envColor = envColor / (envColor + vec3(1.0f));
	envColor = pow(envColor, vec3(1.0/2.2)); 
    
    fragColor = vec4(envColor, 1.0);
}