
#include "hzpch.h"

#include <Hazel.h>



class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer()
		: Layer("Example") {
		
		m_Camera.reset(Hazel::Camera::Create(-2.0, 2.0, -2, 2));
		//m_Camera->SetPosition(glm::vec3(0.5,0.5,0.5));
		//m_Camera->SetRotation(glm::vec4(0., 0., 1., 45.));

		m_VertexArray.reset(Hazel::VerTexArray::Create());

		float vertices[3 * 4] = {
		 -.9,-.9,-.5,//左下
		-.9,.9,-.5,//左上
		.9,-.9,-.5,//you xia
		.9,.9,-.5//you shang
		};

		m_VertexBuffer.reset(Hazel::VerTexBuffer::Creat(vertices, sizeof(vertices)));


		{//把不要的东西全部都销毁
			Hazel::BufferLayout layout = {
				//"addv"是const std::string& 类型或是std::string 也行，但就是千万别std::string&
				{ Hazel::ShaderDataType::Float3, "a_Pos"},
			};
			//设置缓冲布局
			m_VertexBuffer->SetLayout(layout);
		}

		//设置属性
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		//indexbuffer

		uint32_t Index[] = { 0, 1, 2,   // first triangle
		1, 2, 3    // second triangle
		};
		m_IndexBuffer.reset(Hazel::IndexBuffer::Creat(Index, sizeof(Index) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		//绑定第二个状态
		m_SquareVA.reset(Hazel::VerTexArray::Create());
		float bluevertices[3 * 4] = {
		 -.5,-.5,-.50,//左下
		-.5,.5,-.50,//左上
		.5,-.5,-.50,//you xia
		.5,.5,-.50//you shang
		};

		//虽然这里是创建智能指针，但是也是创建类，也是实例化，注意构造函数,不是，没搞懂这个make_shared的用法
		std::shared_ptr<Hazel::VerTexBuffer> squareVB;
		squareVB.reset(Hazel::VerTexBuffer::Creat(bluevertices, sizeof(bluevertices)));

		{//把不要的东西全部都销毁
			Hazel::BufferLayout layout = {
				//"addv"是const std::string& 类型或是std::string 也行，但就是千万别std::string&
				{Hazel::ShaderDataType::Float3, "a_Pos"},
			};
			//设置缓冲布局
			squareVB->SetLayout(layout);
		}

		//设置属性
		m_SquareVA->AddVertexBuffer(squareVB);
		m_SquareVA->SetIndexBuffer(m_IndexBuffer);

		//shader
		std::string VertexSrc = R"(
		#version 330 core
		
		// 输入顶点数据
		layout(location = 0) in vec3 aPos;      // 顶点位置
		// 输出到片段着色器的变量
		out vec2 Pos;
		out vec2 stand;
		uniform vec2 iResolution;
		out vec2 screen;
		uniform mat4 u_ViewProjection;
		
		// 顶点着色器主函数
		void main() {
		    // 设置顶点位置
		    gl_Position = u_ViewProjection * vec4(aPos, 1.0);
			vec4 m_Pos = gl_Position;
		    // 将纹理坐标传递到片段着色器
		
		    Pos = vec2((m_Pos.x+1)*iResolution.x/2,(m_Pos.y+1)*iResolution.y/2);
		    stand=vec2(m_Pos.x,m_Pos.y);
		    screen=iResolution;
		}
)";
		std::string FragmentSrc = R"(
		#version 330 core
			out vec4 FragColor;
			in vec2 Pos;
			in vec2 stand;
			in vec2 screen;
			uniform float iTime;
			
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
				  final+=col*d;
			}
			
			
			FragColor=vec4(final,0.);
			
			}
	
)";

		std::string m_BlueVertexSrc = R"(
		#version 330 core
		
		// 输入顶点数据
		layout(location = 0) in vec3 aPos;      // 顶点位置
		uniform vec2 iResolution;
		uniform mat4 u_ViewProjection;


		// 顶点着色器主函数
		void main() {
		    // 设置顶点位置
		    gl_Position = u_ViewProjection*vec4(aPos, 1.0);
		}
)";
		std::string m_BuleFragmentSrc = R"(
		#version 330 core
			uniform float iTime;
			out vec4 FragColor;	
			void main(){
			FragColor=vec4(0.2f,0.2f,0.6f,0.f);	
			}
)";

		m_Shader.reset(new Hazel::Shader(VertexSrc, FragmentSrc));
		m_BlueShader.reset(new Hazel::Shader(m_BlueVertexSrc, m_BuleFragmentSrc));
	}

	void OnUpdate() override {
		float width  = (float)Hazel::Application::Get().GetWindow().GetWidth();
		float height = (float)Hazel::Application::Get().GetWindow().GetHeight();

		if (Hazel::Input::IsKeyPressed(HZ_KEY_S)) {
			m_Position.y += m_CameraSpeed;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_W)) {
			m_Position.y -= m_CameraSpeed;
		}
		if (Hazel::Input::IsKeyPressed(HZ_KEY_A)) {
			m_Position.x += m_CameraSpeed;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D)) {
			m_Position.x -= m_CameraSpeed;
		}

		if (Hazel::Input::IsKeyPressed(HZ_KEY_E)) {
			m_Rotation += m_RotationSpeed;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_Q)) {
			m_Rotation -= m_RotationSpeed;
		}


		//	m_Camera->SetProjection({ 45.f,ScreenRatio,0.1,100 });

		Hazel::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		Hazel::RenderCommand::Clear();

		m_Camera->SetPosition(m_Position);
		m_Camera->SetRotation(glm::vec4(0.f, 0.f, 1.f, m_Rotation));

		Hazel::Renderer::BeginScene(m_Camera,{width,height});

		Hazel::Renderer::Submit(m_Shader, m_VertexArray);
		Hazel::Renderer::Submit(m_BlueShader, m_SquareVA);
	

		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& event) override {

		////HZ_TRACE("{0}", event);
		
		if (event.GetEventType() == Hazel::EventType::KeyPressed) {
			Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
			//if (e.GetKeyCode() == HZ_KEY_TAB)
			//HZ_TRACE("Tab key is pressed (event)!");
		    HZ_TRACE("{0}", (char)e.GetKeyCode());
			e.Handled = true;
		}

	}

	void OnImGuiRender() override {

	}

private:
	std::shared_ptr<Hazel::Shader> m_Shader;
	std::shared_ptr<Hazel::Shader> m_BlueShader;

	//每个vao 表示一个渲染流程
	std::shared_ptr<Hazel::VerTexBuffer> m_VertexBuffer;
	std::shared_ptr<Hazel::IndexBuffer> m_IndexBuffer;
	std::shared_ptr<Hazel::VerTexArray> m_VertexArray;

	//多个vao
	std::shared_ptr<Hazel::VerTexArray> m_SquareVA;
	//std::unique_ptr<BufferLayout> m_BufferLayout;

	std::shared_ptr<Hazel::Camera> m_Camera;

	glm::vec3 m_Position{ 0.5f };
	float m_CameraSpeed = 0.1f;
	float m_RotationSpeed = 3.f;
	float m_Rotation = 0.f;
};

class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		std::cout << "Example";
	}
	~Sandbox() {

	}
};
 
Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}