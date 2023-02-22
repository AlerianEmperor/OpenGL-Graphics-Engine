#version 430 core

out vec4 fragColor;

in vec2 texcoord;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	vec2 texOffset = 1.0f / textureSize(image, 0);
	vec3 result = texture(image, texcoord).rgb * weight[0];

	float tex_offset_array[2] = {texOffset.y, texOffset.x};

	int ind = horizontal ? 1 : 0;

	for(int i = 1; i < 5; ++i)
	{
		result += texture(image, texcoord + vec2(tex_offset_array[ind] * i, 0.0f)).rgb * weight[i];
		result += texture(image, texcoord - vec2(tex_offset_array[ind] * i, 0.0f)).rgb * weight[i];
	}

	fragColor = vec4(result, 1.0f);
}