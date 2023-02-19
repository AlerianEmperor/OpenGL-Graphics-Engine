#version 430 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;
layout (location = 3) in vec3 i_tangent;
layout (location = 4) in vec3 i_biTangent;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_model_view;

out VS_OUT
{
	vec2 o_texcoord;
	vec3 o_normal;

	vec3 view_pos;
	vec3 light_pos;
	vec3 frag_pos;
} vs_out;

uniform bool use_normal_mapping;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vs_out.o_texcoord = i_texcoord;
		
	vec3 fragPos = vec3(u_model * vec4(i_pos, 1.0f));

	mat3 normalMatrix = transpose(inverse(mat3(u_model)));
		
	vs_out.o_normal = normalMatrix * i_normal;
	
	vs_out.view_pos = (u_model_view * vec4(viewPos, 1.0f)).xyz;
	vs_out.light_pos = lightPos;
	vs_out.frag_pos = fragPos;

	
	gl_Position = u_projection * u_model_view * vec4(i_pos, 1.0f);
}


