#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

// 使用独立命名空间，避免和 fragment shader 冲突
layout(location = 0) out vec3 v_SkyboxLookupDir; 

layout(std140, binding = 1) uniform SkyboxCameraData
{
    mat4 u_StaticViewProjection;
} u_SkyboxCamera;

void main()
{
    // 强制拷贝一份，防止直接使用 a_Position 导致 ID 混淆
    vec3 rawPos = a_Position;
    v_SkyboxLookupDir = rawPos;

    // 深度值欺骗：使用 xyww 确保天空盒在最远平面
    vec4 staticPos = u_SkyboxCamera.u_StaticViewProjection * vec4(rawPos, 1.0);
    gl_Position = staticPos.xyww;
}

#type fragment
#version 450 core

layout(location = 0) in vec3 v_SkyboxLookupDir;
layout(location = 0) out vec4 o_Color;

// 显式绑定 Slot 1
layout(binding = 1) uniform samplerCube u_SkyboxSampler;

void main()
{
// 不管 v_Coord 指向哪，我强行让它去采负 X 方向 (即你看到红色那个面)
    // 注意：负 X 面对应的向量大致是 (-1.0, 0.0, 0.0)
    vec4 testColor = texture(u_SkyboxSampler, vec3(-1.0, 0.0, 0.0));
    
    // 如果屏幕变红了（红+蓝=紫） -> 结论：采样器是好的，是你的向量指向了黑色的面！
    // 如果屏幕还是蓝色 -> 结论：采样器连那个红色的面都读不到（绑定问题）。
    o_Color = testColor + vec4(0.0, 0.0, 0.2, 1.0);

}