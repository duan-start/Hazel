#type vertex
#version 330 core
// 输入顶点数据
layout(location = 0) in vec3 aPos;      // 顶点位置
layout(location = 1) in vec4 aCol;
layout(location = 2) in vec2 aTex;
uniform vec2 iResolution;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
out vec2 TexCoords;
out vec4 V_Col;
// 顶点着色器主函数
void main() {
    // 设置顶点位置
    //gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);

    gl_Position = u_ViewProjection*vec4(aPos, 1.0);
	TexCoords=aTex;
    V_Col=aCol;
}

#type fragment
#version 330 core
uniform float iTime;
out vec4 FragColor;	
in vec2 TexCoords;
in vec4 V_Col;
uniform sampler2D u_texture;
uniform vec4 u_color;
uniform float u_tilingFactor;
void main(){
//FragColor=vec4(TexCoords,0.f,1.0f);
//FragColor=texture(u_texture,TexCoords*u_tilingFactor)*u_color;	

FragColor=texture(u_texture,TexCoords)*V_Col;
//FragColor=vec4(0.5f,0.2f,0.3f,1.0f);

}