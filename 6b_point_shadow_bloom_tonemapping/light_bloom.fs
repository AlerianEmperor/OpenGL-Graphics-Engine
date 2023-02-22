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

//Tone Mapping 

//ACES
vec3 ACES_Input_Mat(vec3 c)
{
	float x = 0.59719f * c.x + 0.35458f * c.y + 0.04823f * c.z;
	float y = 0.07600f * c.x + 0.90834f * c.y + 0.01566f * c.z;
	float z = 0.02840f * c.x + 0.13383f * c.y + 0.83777f * c.z;

	return vec3(x, y, z);
}

vec3 RRTAndODTFit(vec3 c)
{
	vec3 a = c * (c + 0.0245786f) - vec3(0.000090537f);
	vec3 b = c * (0.983729f * c + 0.4329510f) + vec3(0.238081f);
	return a / b;
}

vec3 ACESOutputMat(vec3 c)
{
	float x =  1.60475f * c.x + -0.53108f * c.y + -0.07367f * c.z;
	float y = -0.10208f * c.x +  1.10813f * c.y + -0.00605f * c.z;
	float z = -0.00327f * c.x + -0.07276f * c.y +  1.0760f  * c.z;

	return vec3(x, y, z);
};

//ACES Tone unordered_mapping

vec3 ACES_Tone_mapping(vec3 color)
{
	vec3 c = color;

	c = ACES_Input_Mat(c);

	c = RRTAndODTFit(c);

	c = ACESOutputMat(c);

	return c;
}

//ACES Film
vec3 ACESFilm(vec3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;

	//float x = min(max(0.0f, v.x), 1.0f);

	vec3 value = ((x*(a*x + b)) / (x*(c*x + d) + e));

	float u = min(max(0.0f, value.x), 1.0f);
	float v = min(max(0.0f, value.y), 1.0f);
	float w = min(max(0.0f, value.z), 1.0f);

	return vec3(u, v, w);
}

//Film mic
float clamp(float x)
{
	return x < 0 ? 0 : x > 1 ? 1 : x;
}

float minf(float a, float b)
{
	return a < b ? a : b;
}

float maxf(float a, float b)
{
	return a > b ? a : b;
}

vec3 clamp_vector(vec3 v)
{
	return vec3(clamp(v.x), clamp(v.y), clamp(v.z));
}

vec3 Film(vec3 x)
{
	//version 2

	vec3 A = vec3(0.15);
	vec3 B = vec3(0.50);
	vec3 C = vec3(0.10);
	vec3 D = vec3(0.20);
	vec3 E = vec3(0.02);
	vec3 F = vec3(0.30);

	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;

	//return a;
}

vec3 Filmmic_Tone_mapping(vec3 c)
{
	vec3 a = Film(16 * c);
	vec3 b = Film(vec3(11.2));
	vec3 d = a / b;
	return clamp_vector(d);
}


float F2(float x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;

	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}



vec3 max_vector(vec3 v1, vec3 v2)
{
	return vec3(maxf(v1.x, v2.x), maxf(v1.y, v2.y), maxf(v1.z, v2.z));
}

vec3 Filmic_unchart2(vec3 v)
{
	vec3 x = max_vector(v - vec3(0.004f), vec3(0.0f));
	return vec3((x * (6.2f * x + 0.5f)) / ( x * (6.2f * x + 1.7f) + 0.06f));
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

	vec3 spec = vec3(pow(max(dot(half_vector, o_normal) , 0.0f), 16.0f));

	//fragColor = vec4(color * 0.05 +  (1.0f - shadow) * (diffuse + spec * scale),  1.0f);

	//fragColor = vec4(shadow);

	vec3 result = (color * 0.05 +  (1.0f - shadow) * 40000 * (diffuse + spec /** scale*/)) / (current_Depth * current_Depth);

	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

	BrightColor = brightness >= 1.0f ? vec4(result, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);

	//result = ACES_Tone_mapping(result);
	result = ACESFilm(result);
	//result = Filmmic_Tone_mapping(result);
	//result = Filmic_unchart2(result);

	fragColor = vec4(result, 1.0f);
}
