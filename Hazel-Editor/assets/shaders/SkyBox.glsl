#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

//因为这个不能冲突BeginScene里面的全局相机的UnformBuffer(binding=0)
//binding=3 是天空盒专用槽（Renderer2D=0，Renderer3D=0/1/2）
layout(std140,binding = 3) uniform Camera
{
    mat4 u_InverseVP;
};

layout(location=0) out vec3 v_Position;

void main()
{
    vec4 position = vec4(a_Position.xy, 1.0, 1.0);
    gl_Position = position;

    v_Position = (u_InverseVP * position).xyz;
}

#type fragment
#version 450

layout(location = 0) out vec4 finalColor;

layout(binding = 0) uniform samplerCube u_Texture;

layout(location=0) in vec3 v_Position;

void main()
{
    finalColor = texture(u_Texture, v_Position);
    // 调试用红色叠加，方便确认天空盒在渲染
    finalColor += vec4(0.3f, 0.1f, 0.1f, 1.0f);
}
