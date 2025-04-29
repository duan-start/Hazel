#type vertex
#version 330 core
		
		// ���붥������
		layout(location = 0) in vec3 aPos;      // ����λ��
		layout(location = 1) in vec2 aTex;
		uniform vec2 iResolution;
		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;
		out vec2 TexCoords;
		// ������ɫ��������
		void main() {
		    // ���ö���λ��
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