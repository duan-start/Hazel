#type vertex
#version 330 core
// 输入顶点数据
layout(location = 0) in vec3 a_Pos;      // 顶点位置
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

// 顶点着色器主函数
void main() {
    // 设置顶点位置
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Pos, 1.0);
}

#type fragment
#version 330 core
uniform vec4 Color;
out vec4 FragColor;	
void main(){
FragColor=Color;	
}
