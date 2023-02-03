#version 430 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_light_space_matrix;

out vec2 o_texcoord;
out vec3 o_normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 view_direction;
out vec3 light_direction;
out vec3 half_vector;

out vec3 frag_pos;

void main()
{
	o_texcoord = i_texcoord;
	
	o_normal = (transpose(inverse(u_model)) * vec4(i_normal, 1.0f)).xyz;
	
	mat4 u_model_view = u_view * u_model;

	vec3 eye_view_space = (u_model_view * vec4(viewPos, 1.0f)).xyz;

	view_direction = normalize(eye_view_space - i_pos);

	//view_direction = normalize(viewPos - i_pos);

	light_direction = normalize(lightPos - i_pos);

	half_vector = normalize(view_direction + light_direction);

	gl_Position = u_projection * u_model_view * vec4(i_pos, 1.0f);

	frag_pos = (u_model * vec4(i_pos, 1.0f)).xyz;
}
