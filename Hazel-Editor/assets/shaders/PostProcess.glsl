#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(a_Position.xy, 0.0, 1.0);
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 450

layout(location = 0) out vec4 finalColor;
layout(location = 0) in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_SceneColor;
layout(std140, binding = 4) uniform PostProcessUBO
{
	vec4 u_Settings; // x = exposure
};

vec3 ACESFilm(vec3 x)
{
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;
	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

void main()
{
	vec3 hdrColor = texture(u_SceneColor, v_TexCoord).rgb;

	// 1. exposure: scale before tonemap
	hdrColor *= u_Settings.x;

	// 2. tonemap: compress HDR into [0,1]
	vec3 mapped = ACESFilm(hdrColor);

	// 3. gamma: encode for sRGB display
	vec3 ldrColor = pow(mapped, vec3(1.0f / 2.2f));

	finalColor = vec4(ldrColor, 1.0f);
}
