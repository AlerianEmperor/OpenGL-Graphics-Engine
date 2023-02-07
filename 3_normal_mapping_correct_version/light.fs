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


void main()
{
	vec3 color = texture(diffuse_texture, o_texcoord).rgb;

	vec3 diffuse = max(0.0f, dot(light_direction, o_normal)) * color;
	vec3 spec = vec3(pow(max(dot(half_vector, o_normal) * 0.9f, 0.0f), 8.0f));

	//float shadowCalculation()

	fragColor = vec4(color * 0.1 + diffuse + spec, 1.0f);
}

/*in vec3 light_Pos;
in vec3 view_Pos;
in vec3 frag_Pos;

void main()
{
	vec3 normal = normalize(o_normal);
	vec3 light_direction = normalize(light_Pos - frag_Pos);
	vec3 view_direction = normalize(view_Pos - frag_Pos);

	vec3 half_vector = normalize(light_direction + view_direction);

	vec3 color = texture(diffuse_texture, o_texcoord).rgb;

	vec3 diffuse = max(0.0f, dot(light_direction, normal)) * color;
	vec3 spec = vec3(pow(max(dot(half_vector, normal) * 0.9f, 0.0f), 8.0f));

	fragColor = vec4(color * 0.05 + diffuse + spec * 0.3, 1.0f);
}*/


