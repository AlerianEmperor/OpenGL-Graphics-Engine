#version 430 core

in vec2 o_texcoord;
in vec3 o_normal;

uniform sampler2D diffuse_texture;
uniform samplerCube shadow_texture;

out vec4 fragColor;

in vec3 view_direction;
in vec3 light_direction;
in vec3 half_vector;

in vec3 frag_pos;

uniform float far_plane;
uniform vec3 lightPos;
uniform vec3 viewPos;

float shadowCalculation(vec3 fragPos)
{
	vec3 fragToLight = fragPos - lightPos;

	//float closest_Depth = texture(shadow_texture, fragPos).r;

	float closest_Depth = texture(shadow_texture, fragToLight).r;

	closest_Depth *= far_plane;

	float current_Depth = length(fragToLight);

	float bias = 9.0;

	float shadow =  current_Depth - bias >= closest_Depth ? 1.0f : 0.0f;

	return shadow;
}

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float pcf(vec3 fragPos, vec3 frag_to_light)
{
	//vec3 frag_to_light = fragPos - lightPos;

	float current_Depth = length(frag_to_light);

	float shadow = 0.0f;
	float bias = 9.5f;

	int samples = 20;

	float viewDistance = length(viewPos - fragPos);

	float diskRadius = (1.0f + (viewDistance / far_plane)) / 25.0f;

	for(int i = 0; i < samples; ++i)
	{
		float closest_Depth = texture(shadow_texture, frag_to_light + gridSamplingDisk[i] * diskRadius).r;

		closest_Depth *= far_plane;

		if(current_Depth - bias > closest_Depth)
			shadow += 1.0f;
	}
	return shadow /= float(samples);
	
	//return shadow;
}

/*float shadowCalculation_bias(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	projCoords = projCoords * 0.5f + 0.5f;

	//if(projCoords.z > 1.0f)
	//	return 0.0f;

	float bias = 0.005f;

	//float bias = max(0.05f * (1.0f - dot(light_direction, o_normal)), 0.05f);

	float closest_Depth = texture(shadow_texture, projCoords.xy).r;

	float current_Depth = projCoords.z;

	float shadow =  current_Depth - bias >= closest_Depth ? 1.0f : 0.0f;

	return shadow;
}

float pcf(vec4 fragPosLightSpace)
{
	//-1 1 space
	//device convert to NDC space
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	//convert to [0, 1]
	projCoords = projCoords * 0.5f + 0.5f;

	//blur radius
	int r = 2;

	float shadow = 0.0f;
	vec2 texel_size = 1.0f / textureSize(shadow_texture, 0);

	float bias = 0.005f;

	float current_Depth = projCoords.z;

	for(int x = -r; x <= r; ++x)
	{
		for(int y = -r; y <= r; ++y)
		{
			float pcfDepth = texture(shadow_texture, projCoords.xy + vec2(x, y) * texel_size).r;

			shadow += current_Depth - bias >= pcfDepth ? 1.0f : 0.0f;
		}
	}

	return shadow * 0.04f;
}*/

void main()
{
	vec3 color = texture(diffuse_texture, o_texcoord).rgb;


	//float shadow = shadowCalculation(frag_pos);

	//float shadow = shadowCalculation_bias(frag_pos_light_space);

	//float shadow = pcf(frag_pos_light_space);

	vec3 frag_to_light = frag_pos - lightPos;

	float current_Depth = length(frag_to_light);

	float shadow = pcf(frag_pos, frag_to_light);

	float scale = 200000.0f / (current_Depth * current_Depth);

	vec3 diffuse = max(0.0f, dot(light_direction, o_normal)) * color;

	vec3 spec = vec3(pow(max(dot(half_vector, o_normal) * 0.9f, 0.0f), 8.0f));

	fragColor = vec4(color * 0.3 +  (1.0f - shadow) * (diffuse + spec * scale),  1.0f);

	//fragColor = vec4(shadow);
}
