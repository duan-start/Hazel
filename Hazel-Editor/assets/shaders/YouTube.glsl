#type vertex
#version 330 core

// 输入顶点数据
layout(location = 0) in vec3 aPos;      // 顶点位置
// 输出到片段着色器的变量
layout(location=1) in vec2 aTex;
out vec2 Pos;
out vec2 stand;
uniform vec2 iResolution;
uniform mat4 u_Transform;
out vec2 screen;
uniform mat4 u_ViewProjection;

// 顶点着色器主函数
void main() {
    // 设置顶点位置
    gl_Position = u_ViewProjection *u_Transform* vec4(aPos, 1.0);
	vec4 m_Pos = gl_Position;
    // 将纹理坐标传递到片段着色器

    Pos = vec2((m_Pos.x+1)*iResolution.x/2,(m_Pos.y+1)*iResolution.y/2);
    stand=vec2(m_Pos.x,m_Pos.y);
    screen=iResolution;
}

#type fragment
#version 330 core
out vec4 FragColor;
in vec2 Pos;
in vec2 stand;
in vec2 screen;
uniform float iTime;
uniform vec4 color;

#define t iTime
#define r screen.xy

vec3 palette(float t){
	vec3 a=vec3 (.5,.5,.5);
	vec3 b=vec3 (.5,.5,.5);
	vec3 c=vec3 (1.,1.,1.);
	vec3 d=vec3 (0.263,.416,.557);
    return a+b*cos( 6.28318*(c*t+d));
    }
	
void main(){
	vec2 uv =stand;
	uv.x*=screen.x/screen.y;
	vec2 uv0=uv;
	vec3 final=vec3(0.,0.,0.);
	
	for(float i=0.;i<4.;i++){
	  uv=fract(uv*1.5)-0.5;
	  float d=length(uv)*exp(-length(uv0));
	  
	  vec3 col=palette(length(uv0)+i*.4+t*.4);
	  d = sin(d*8.+t)/8.;
	  d=abs(d);
	  //d=step(0.1,d);
	  
	  //smoothstep 函数，数值之间的平滑过度
	  // d=smoothstep(0.,0.1,d);

	  d=pow(0.01/d,1.2);
	  final+=col * d;
}
	vec3 Color=vec3(color.x,color.y,color.z);
	final=(final+Color)/2;
	final=clamp(final+Color,0,1);
	FragColor=vec4(final,color.w);
	//FragColor=color;
	}
	