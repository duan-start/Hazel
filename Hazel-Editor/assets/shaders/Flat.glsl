#type vertex
#version 330 core
// ���붥������
layout(location = 0) in vec3 a_Pos;      // ����λ��
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

// ������ɫ��������
void main() {
    // ���ö���λ��
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Pos, 1.0);
}

#type fragment
#version 330 core
uniform vec4 Color;
out vec4 FragColor;	
void main(){
FragColor=Color;	
}
