#type vertex
#version 330 core
		
		// 输入顶点数据
		layout(location = 0) in vec3 aPos;      // 顶点位置
		layout(location = 1) in vec2 aTex;
		uniform vec2 iResolution;
		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;
		out vec2 TexCoords;
		// 顶点着色器主函数
		void main() {
		    // 设置顶点位置
		    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
			TexCoords=aTex;
		}

#type fragment
#version 330 core
			uniform float iTime;
			out vec4 FragColor;	
			in vec2 TexCoords;
			uniform sampler2D u_texture;
			void main(){
			FragColor=texture(u_texture,TexCoords);	
			//FragColor=vec4(fragColor.x,fragColor.y,fragColor.z,0.5);
			}