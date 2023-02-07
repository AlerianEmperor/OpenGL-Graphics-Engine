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

	vec3 tangent_view_pos;
	vec3 tangent_light_pos;

	vec3 view_pos;
	vec3 light_pos;
	vec3 frag_pos;
	vec3 tangent_frag_pos;

	vec3 light_direction;
	vec3 view_direction;
	mat3 TBN;
} vs_out;

uniform bool use_normal_mapping;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vs_out.o_texcoord = i_texcoord;
	
	
	vec3 fragPos = vec3(u_model * vec4(i_pos, 1.0f));

	vec3 v = (u_model_view * vec4(viewPos, 1.0f)).xyz - fragPos;
	vec3 l = lightPos - fragPos;

	if(use_normal_mapping)
	{
		//start normal mapping part
	
		mat3 normalMatrix = transpose(inverse(mat3(u_model)));

		vec3 T = normalize(normalMatrix * i_tangent);
		vec3 N = normalize(normalMatrix * i_normal);

		//vec3 B = normalize(normalMatrix * i_biTangent);

		//vec3 N = cross(T, B);

		T = normalize(T - dot(T, N) * N);

		vec3 B = cross(N, T);

		//if(dot(cross(T, B), N) < 0)
		//	B = -B;

		mat3 TBN = transpose(mat3(T, B, N));

		//vs_out.TBN = TBN;

		//end normal mapping part
	
		vs_out.light_pos = TBN * lightPos;
		vs_out.view_pos = TBN * viewPos;
		vs_out.frag_pos = TBN * fragPos;

		//vs_out.frag_pos = fragPos;

		//vs_out.o_normal = i_normal.xyz;
		//vs_out.o_normal = TBN * i_normal.xyz;

		/*vs_out.view_pos = TBN * (u_model_view * vec4(viewPos, 1.0f)).xyz;
		vs_out.light_pos = TBN * lightPos;
		vs_out.frag_pos = TBN * (u_model * vec4(fragPos, 1.0f)).xyz;//*/

		//vs_out.view_pos = vec3(u_model_view * vec4(TBN * viewPos, 1.0f));
		//vs_out.light_pos = TBN * lightPos;
		//vs_out.frag_pos = vec3(u_model * (TBN  * fragPos, 1.0f));

		//vs_out.view_pos = (u_model_view * vec4(viewPos, 1.0f)).xyz;//vec3(u_model_view * vec4(TBN * viewPos, 1.0f));
		//vs_out.light_pos = lightPos;
		//vs_out.frag_pos = vec3(u_model * vec4(fragPos, 1.0f));

		

		//vs_out.view_direction = vec3(dot(T, v), dot(B, v), dot(N, v));
		//vs_out.light_direction = vec3(dot(T, l), dot(B, l), dot(N, l));
	}
	else
	{
		mat3 normalMatrix = transpose(inverse(mat3(u_model)));
		
		vs_out.o_normal = normalMatrix * i_normal;
		
		//vs_out.o_normal = (u_model_view * vec4(normalMatrix * i_normal, 1.0f)).xyz;
		
		vs_out.view_pos = (u_model_view * vec4(viewPos, 1.0f)).xyz;
		vs_out.light_pos = lightPos;
		vs_out.frag_pos = (u_model * vec4(i_pos, 1.0f)).xyz;//(u_model_view * vec4(i_pos, 1.0f)).xyz;

		//vs_out.view_direction = v;//(u_model_view * vec4(v, 1.0f)).xyz;
		//vs_out.light_direction = l - fragPos;//(  vec4(l, 1.0f)).xyz;//vec3(dot(T, l), dot(B, l), dot(N, l));
	}
	gl_Position = u_projection * u_model_view * vec4(i_pos, 1.0f);
}


