//--------------------------
// - Hazel 2D -
// Renderer2D Circle Shader
// --------------------------

#type vertex
#version 450 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Fade;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;

void main()
{
	Output.LocalPosition = a_LocalPosition;
	Output.Color = a_Color;
	Output.Thickness = a_Thickness;
	Output.Fade = a_Fade;

	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;

void main()
{
	//smoothstep是用来消除锯齿的，整个范围的一个映射
    // Calculate distance and fill circle with white
    float distance = 1.0 - length(Input.LocalPosition);
	//外圈smooth
    float circle = smoothstep(0.0, Input.Fade, distance);
	//内圈smooth 和半径确定，（做了一个取放）
    circle *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);

//这样能在对应的frambuffer上不输出，而不仅仅是输出黑色
	if (circle == 0.0)
		discard;

    // Set output color
    o_Color = Input.Color;
	//ͨ������͸���ȵĴ�С����Բ����С
	o_Color.a *= circle;

	o_EntityID = v_EntityID;
}