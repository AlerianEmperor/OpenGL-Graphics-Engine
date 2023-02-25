#version 430 core

out vec4 fragColor;
in vec3 worldPos;

uniform sampler2D rectangle_map;

const vec2 invAtan = vec2(0.1591, 0.3181);

vec2 sampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
	vec2 uv = sampleSphericalMap(normalize(worldPos));
	vec3 color = texture(rectangle_map, uv).rgb;

	fragColor = vec4(color, 1.0f);
}
