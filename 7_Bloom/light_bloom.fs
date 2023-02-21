#version 430 core

in vec2 o_texcoord;
in vec3 o_normal;

uniform sampler2D diffuse_texture;
uniform samplerCube shadow_texture;

//out vec4 fragColor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 BrightColor;

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

float shadow_offset_scale = 0.1f;

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
		float closest_Depth = texture(shadow_texture, frag_to_light + gridSamplingDisk[i] * shadow_offset_scale /*diskRadius*/ ).r;

		closest_Depth *= far_plane;

		if(current_Depth - bias > closest_Depth)
			shadow += 1.0f;
	}
	return shadow /= float(samples);
	
	//return shadow;
}

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

	//fragColor = vec4(color * 0.05 +  (1.0f - shadow) * (diffuse + spec * scale),  1.0f);

	//fragColor = vec4(shadow);

	vec3 result = color * 0.05 +  (1.0f - shadow) * (diffuse + spec * scale);

	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

	BrightColor = brightness > 1.0f ? vec4(result, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);

	fragColor = vec4(result, 1.0f);
}
