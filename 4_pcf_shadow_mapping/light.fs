#version 430 core

in vec2 o_texcoord;
in vec3 o_normal;

uniform sampler2D diffuse_texture;
uniform sampler2D shadow_texture;

out vec4 fragColor;

in vec3 view_direction;
in vec3 light_direction;
in vec3 half_vector;
in vec3 frag_pos;
in vec4 frag_pos_light_space;

float shadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	projCoords = projCoords * 0.5f + 0.5f;

	//float closest_Depth = texture(shadow_texture, o_texcoord).r;

	float closest_Depth = texture(shadow_texture, projCoords.xy).r;

	float current_Depth = projCoords.z;

	float shadow =  current_Depth > closest_Depth ? 1.0f : 0.0f;

	return shadow;
}

float shadowCalculation_bias(vec4 fragPosLightSpace)
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

/*float pcf(vec4 fragPosLightSpace)
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

float pcf(vec4 fragPosLightSpace)
{
	//-1 1 space
	//device convert to NDC space
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	//convert to [0, 1]
	projCoords = projCoords * 0.5f + 0.5f;

	//blur radius
	int r = 6;

	float shadow = 0.0f;
	vec2 texel_size = 1.0f / textureSize(shadow_texture, 0);

	float bias = 0.02f;

	//float bias = max(0.05f * (1.0f - dot(light_direction, o_normal)), 0.05f);

	float current_Depth = projCoords.z;

	for(int x = -r; x <= r; ++x)
	{
		for(int y = -r; y <= r; ++y)
		{
			float pcfDepth = texture(shadow_texture, projCoords.xy + vec2(x, y) * texel_size).r;

			shadow += current_Depth - bias >= pcfDepth ? 1.0f : 0.0f;
		}
	}

	return shadow / ((2 * r + 1) * (2 * r + 1));
}

void main()
{
	vec3 color = texture(diffuse_texture, o_texcoord).rgb;

	vec3 diffuse = max(0.0f, dot(light_direction, o_normal)) * color;
	vec3 spec = vec3(pow(max(dot(half_vector, o_normal) * 0.9f, 0.0f), 8.0f));

	//float shadow = shadowCalculation(frag_pos_light_space);

	//float shadow = shadowCalculation_bias(frag_pos_light_space);

	float shadow = pcf(frag_pos_light_space);

	fragColor = vec4(color * 0.1 +  (1.0f - shadow) * (diffuse + spec), 1.0f);

	//fragColor = vec4(shadow);
}
