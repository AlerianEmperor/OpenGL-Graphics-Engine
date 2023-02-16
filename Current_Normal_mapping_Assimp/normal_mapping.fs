#version 430 core

 uniform layout(binding = 0) sampler2D diffuse_texture;
 //uniform sampler2D diffuse1_texture;
 //uniform sampler2D diffuse2_texture;
 //uniform sampler2D diffuse3_texture;

 uniform layout(binding = 1) sampler2D normal_texture;

 

out vec4 fragColor;

in VS_OUT
{
	vec2 o_texcoord;
	vec3 o_normal;

	vec3 view_pos;
	vec3 light_pos;
	vec3 frag_pos;
	
	
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
		normal = (texture(normal_texture, fs_in.o_texcoord).rgb);
		normal = normalize(2.0f * normal - 1.0f);

		//normal.y = - normal.y;
	}


	
	vec3 light_direction = normalize(fs_in.light_pos - fs_in.frag_pos);
	vec3 view_direction  = normalize(fs_in.view_pos - fs_in.frag_pos);

	vec3 half_vector = normalize(light_direction + view_direction);

	vec3 ambient = color * 0.05;
	vec3 diffuse = max(0.0f, dot(light_direction, normal)) * color;
	vec3 spec = vec3(pow(max(dot(half_vector, normal), 0.0f), 32.0f)) * 0.2f;

	

	fragColor = vec4(ambient + diffuse + spec, 1.0f);

	//fragColor = vec4(normal, 1.0f);

	//fragColor = vec4(color, 1.0f);
	
}