#version 430 core

out vec4 fragColor;
in vec3 worldPos;

uniform samplerCube enviromentMap;

const float pi = 3.1415926;


void main()
{
	vec3 normal = normalize(worldPos);
	vec3 irradiance = vec3(0.0);  

	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up         = normalize(cross(normal, right));

	float sampleDelta = 0.025;
	float nrSamples = 0.0; 
	for(float phi = 0.0; phi < 2.0 * pi; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * pi; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

			irradiance += texture(enviromentMap, sampleVec).rgb * cos(theta) * sin(theta);
			++nrSamples;
		}
	}
	irradiance = pi * irradiance * (1.0f / (nrSamples));

	/*vec3 envColor = texture(enviromentMap, worldPos).rgb;

	envColor = envColor / (envColor + vec3(1.0f));
	envColor = pow(envColor, vec3(1.0/2.2)); 
    
	fragColor = vec4(envColor, 1.0);*/

    fragColor = vec4(irradiance, 1.0);
}