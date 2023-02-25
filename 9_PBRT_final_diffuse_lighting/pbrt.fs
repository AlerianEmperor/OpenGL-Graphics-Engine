#version 430 core

out vec4 fragColor;

in vec2 texcoord;
in vec3 worldPos;
in vec3 normal;

//uniform vec3 albedo;
//uniform float metallic;
//uniform float roughness;
//uniform float ao;

uniform sampler2D albedo_texture;
uniform sampler2D ao_texture;
uniform sampler2D metallic_texture;
uniform sampler2D roughness_texture;

uniform samplerCube irradiance_texture;

uniform float u_roughness;

uniform vec3 lightPositions[4]; 
uniform vec3 lightColors[4];
uniform vec3 viewPos;

const float pi = 3.1415926;

//D
float GGX_Trowbridge(vec3 m, vec3 n, float roughness)
{
	//float a1 = a + 1;
	
	//a1 = a1 * a1 / 8; 
	float a = roughness * roughness;
	float a2 = a * a;
	float cos_theta = max(dot(m, n), 0.0f);
	float cos_theta2 = cos_theta * cos_theta;

	float denom = cos_theta2 * (a2 - 1) + 1;
	float denom2 = denom * denom;

	return a2 / (pi * denom2);
}

//G
//G = G1(v) * G1(l)
float G1(vec3 v, vec3 n, float a)
{
	/*float cos_t = dot(v, n);
	float a2 = a * a;

	return 2 * cos_t / (cos_t + sqrt(a2 + (1.0f - a2) * cos_t * cos_t));*/

	float r = a + 1;
	float k = r * r / 8.0f;

	float cos_t = max(dot(v, n), 0.0f);

	return cos_t / (cos_t * (1.0f - k) + k);
}

float Geometry(vec3 v, vec3 l, vec3 n, float a)
{
	return G1(v, n, a) * G1(l, n, a);
}

//F 
vec3 Fresnel_Schlick(vec3 v, vec3 h, vec3 F0)
{
	float cos_t = clamp(1.0f - dot(v, h), 0.0f, 1.0f);

	float cos_t2 = cos_t * cos_t;

	return vec3(F0 + (1.0f - F0) * (cos_t * cos_t2 * cos_t2));
}

void main()
{
	vec3 n = normalize(normal);

	vec3 v = normalize(viewPos - worldPos);

	vec3 F0 = vec3(0.04);

	vec3 albedo = texture(albedo_texture, texcoord).rgb;
	vec3 ao = texture(ao_texture, texcoord).rgb;
	vec3 metallic = texture(metallic_texture, texcoord).rgb;
	float roughness = u_roughness;//texture(roughness_texture, texcoord).;



	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0f);

	for(int i = 0; i < 4; ++i)
	{
		vec3 l = normalize(lightPositions[i] - worldPos);
		vec3 h = normalize(v + l);

		float distance = length(lightPositions[i] - worldPos);

		float attenuate = 1.0f / (distance * distance);

		vec3 radiance = lightColors[i] * attenuate;

		float D = GGX_Trowbridge(h, n, roughness);

		float G = Geometry(v, l, n, roughness);

		vec3 F = Fresnel_Schlick(v, h, F0);

		vec3 numerator = D * G * F;

		float denominatior = 4.0f * max(dot(v, n), 0.0f) * max(dot(l, n), 0.0f)  + 0.0001f;
	
		vec3 specular = numerator / denominatior;

		vec3 Ks = F;

		//vec3 Kd = vec3(1.0f);

		vec3 Kd = vec3(1.0f) - Ks;
		Kd *= 1.0f - metallic;

		float cos_ln = max(dot(l, n), 0.0f);

		Lo += (Kd * albedo / pi + specular) * radiance * cos_ln;
	}

	/*vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 color = ambient + Lo;*/

	vec3 Ks = Fresnel_Schlick(v, n, F0);
	vec3 Kd = vec3(1.0f) - Ks;
	Kd *= 1.0f - metallic;

	vec3 irrandiance = texture(irradiance_texture, n).rgb;
	vec3 diffuse = irrandiance * albedo;
	vec3 ambient = (Kd * diffuse) * ao;

	//vec3 color = ambient + diffuse;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0f));

	color = pow(color, vec3(1.0f / 2.0f));

	fragColor = vec4(color, 1.0f);

}
