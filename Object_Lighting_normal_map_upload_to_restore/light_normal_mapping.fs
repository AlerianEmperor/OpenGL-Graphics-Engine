#version 430 core

layout(binding = 0) uniform sampler2D diffuse_texture;
layout(binding = 1) uniform sampler2D normal_texture;

out vec4 fragColor;

in VS_OUT
{
	vec2 o_texcoord;
	vec3 o_normal;

	vec3 view_pos;
	vec3 light_pos;
	vec3 frag_pos;
	mat3 TBN;
} fs_in;

uniform bool use_normal_mapping;

void main()
{
	vec3 color = texture(diffuse_texture, fs_in.o_texcoord).rgb;

	vec3 normal;

	if(!use_normal_mapping)
		normal = normalize(fs_in.o_normal);
	else
	{
		//normal = fs_in.o_normal;
		normal = texture(normal_texture, fs_in.o_texcoord).rgb;// / 255.0f;// - vec3(97, 97, 97);
		
		//normal = normalize(normal );
		normal = normalize(2.0f * normal - 1.0f);
		//normal.y = -normal.y;
		//normal = fs_in.TBN * normal;


		//normal = normalize(2.0f * normal - vec3(1.0f));

		//normal = fs_in.TBN * normal;
		//normal = (2.0f * normal - vec3(1.0f));
		//normal = normalize(vec3(1.0f) - 2.0f * normal);

		//normal = -normalize(vec3(2.0f * normal.x - 1.0f, 1.0f - 2.0f * normal.y, 2.0f * normal.z - 1.0f));
	}


	vec3 light_direction = normalize(fs_in.light_pos - fs_in.frag_pos);
	vec3 view_direction = normalize(fs_in.view_pos - fs_in.frag_pos);
	
	vec3 half_vector = normalize(light_direction + view_direction);

	vec3 diffuse = max(0.0f, dot(light_direction, normal)) * color;
	vec3 spec = vec3(pow(max(dot(half_vector, normal) * 0.9f, 0.0f), 32.0f));

	fragColor = vec4(color * 0.1 + diffuse + spec, 1.0f);
	//fragColor = vec4(normal, 1.0f);
}