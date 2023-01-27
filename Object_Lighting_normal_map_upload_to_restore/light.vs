#version 430 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_model_view;

out vec2 o_texcoord;
out vec3 o_normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 view_direction;
out vec3 light_direction;
out vec3 half_vector;

void main()
{
	o_texcoord = i_texcoord;
	o_normal = i_normal.xyz;//(transpose(inverse(u_model)) * vec4(i_normal, 1.0f)).xyz;
	
	vec3 position = i_pos.xyz;//(u_model_view * vec4(i_pos, 1.0f)).xyz;

	vec3 light_view_space = lightPos.xyz;//(u_model_view * vec4(lightPos, 1.0f)).xyz;

	vec3 eye_view_space = (u_model_view * vec4(viewPos, 1.0f)).xyz;

	view_direction = normalize(eye_view_space - position);

	light_direction = normalize(light_view_space - position);

	half_vector = normalize(view_direction + light_direction);

	gl_Position = u_projection * u_model_view * vec4(i_pos, 1.0f);//u_projection * u_view * u_model * vec4(i_pos, 1.0f);
}


/*out vec3 light_Pos;
out vec3 view_Pos;
out vec3 frag_Pos;

void main()
{
	o_texcoord = i_texcoord;
	o_normal = i_normal;

	light_Pos = lightPos;
	view_Pos = viewPos;

	gl_Position = u_projection * u_model_view * vec4(i_pos, 1.0f);
}*/
