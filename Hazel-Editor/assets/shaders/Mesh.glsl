
#type vertex
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec3 Tan;
layout(location = 3) in vec3 Bin;
layout(location = 4) in vec2 aTex;

layout(std140, binding = 1) uniform TransformData
{
	mat4 u_Model;
};

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};
layout(location=0) out vec2 pTex;

void main()
{

    // a_Pos 通常在 -1 到 1 之间，这会在屏幕正中央画一个大白块
    gl_Position = u_ViewProjection*vec4(aPos, 1.0);
	pTex=aTex;
}


#type fragment
#version 450 core
//原始的不做抗锯齿真的很难看
layout(location = 0) out vec4 o_Color;

layout(location=0) in vec2 pTex;

layout(binding = 4) uniform sampler2D u_Textures[32];
void main()
{

	 o_Color = texture(u_Textures[0], vec2(pTex.x,1.0-pTex.y));
}