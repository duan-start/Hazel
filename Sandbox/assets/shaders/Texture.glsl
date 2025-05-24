#type vertex
#version 330 core
// 输入顶点数据
layout(location = 0) in vec3 aPos;      // 顶点位置
layout(location = 1) in vec4 aCol;
layout(location = 2) in vec2 aTex;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;
//uniform mat4 u_Transform;
out vec2 V_TexCoord;
out vec4 V_Col;
out float V_TexIndex;
out float V_Tili;

// 顶点着色器主函数
void main() {
    // 设置顶点位置
    //gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);

    //SetPosition
    gl_Position = u_ViewProjection * vec4(aPos, 1.0);
    
	V_TexCoord=aTex;
    V_Col=aCol;
    V_TexIndex=a_TexIndex;
    V_Tili=a_TilingFactor;
}

#type fragment
#version 330 core
out vec4 FragColor;	

in vec2 V_TexCoord;
in vec4 V_Col;
in float V_TexIndex;
in float V_Tili;

uniform sampler2D u_texture[32];
//uniform vec4 u_color;
//uniform float u_tilingFactor;
void main(){
//FragColor=vec4(TexCoords,0.f,1.0f);
//FragColor=texture(u_texture,TexCoords*u_tilingFactor)*u_color;	

FragColor=texture(u_texture[int(V_TexIndex)],V_TexCoord*V_Tili)*V_Col;
//FragColor=vec4(0.5f,0.2f,0.3f,1.0f);

}