#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

layout(std140,binding = 2) uniform Camera
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

layout(binding = 3) uniform samplerCube u_Texture;

layout(location=0) in vec3 v_Position;

void main()
{
    finalColor = texture(u_Texture, v_Position);
    finalColor+=vec4(0.3f,0.1f,0.1f,1.0f);
}
