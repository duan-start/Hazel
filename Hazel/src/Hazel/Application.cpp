#include "hzpch.h"

#include "Application.h"
#include"Events/ApplicationEvent.h"
#include"Log.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Hazel {



	//这个的用法
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		HZ_CORE_ASSERT(!s_Instance, "Application has been existed ");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		//按理是要在析构函数中delete掉的，
		m_ImGuiLayer = new ImguiLayer();
		PushOverlay(m_ImGuiLayer);

		m_VertexArray.reset(VerTexArray::Create());
	
		float vertices[3 * 4] = {
		 -.9,-.9,0,//左下
		-.9,.9,0,//左上
		.9,-.9,0,//you xia
		.9,.9,0//you shang
		};

		m_VertexBuffer.reset(VerTexBuffer::Creat(vertices, sizeof(vertices)));
		

		{//把不要的东西全部都销毁
			BufferLayout layout = {
				//"addv"是const std::string& 类型或是std::string 也行，但就是千万别std::string&
				{ ShaderDataType::Float3, "a_Pos"},
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
		m_IndexBuffer.reset(IndexBuffer::Creat(Index, sizeof(Index) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		//绑定第二个状态
		m_SquareVA.reset(VerTexArray::Create());
		float bluevertices[3 * 4] = {
		 -1.,-1.,0,//左下
		-1.,1.,0,//左上
		1.,-1.,0,//you xia
		1.,1.,0//you shang
		};

		//虽然这里是创建智能指针，但是也是创建类，也是实例化，注意构造函数,不是，没搞懂这个make_shared的用法
		std::shared_ptr<VerTexBuffer> squareVB;
		squareVB.reset(VerTexBuffer::Creat(bluevertices, sizeof(bluevertices)));

		{//把不要的东西全部都销毁
			BufferLayout layout = {
				//"addv"是const std::string& 类型或是std::string 也行，但就是千万别std::string&
				{ ShaderDataType::Float3, "a_Pos"},
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
		
		// 顶点着色器主函数
		void main() {
		    // 设置顶点位置
		    gl_Position = vec4(aPos, 1.0);
		
		    // 将纹理坐标传递到片段着色器
		
		    Pos = vec2((aPos.x+1)*iResolution.x/2,(aPos.y+1)*iResolution.y/2);
		    stand=vec2(aPos.x,aPos.y);
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


		// 顶点着色器主函数
		void main() {
		    // 设置顶点位置
		    gl_Position = vec4(aPos, 1.0);
		}
)";
		std::string m_BuleFragmentSrc = R"(
		#version 330 core
			out vec4 FragColor;	
			void main(){
			FragColor=vec4(0.2f,0.2f,0.6f,0.f);	
			}
)";

		m_Shader.reset(new Shader(VertexSrc,FragmentSrc));
		m_BlueShader.reset(new Shader(m_BlueVertexSrc, m_BuleFragmentSrc));

	}

	Application:: ~Application() {
		//不用写，这个实际上实在laystack手动删除，在那个地方管理生命周期
		//delete m_ImGuiLayer;  // 手动释放 m_ImGuiLayer
	}

	//设置事件回调函数
	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClose>(BIND_EVENT_FN(Application::OnWindowClose));
		//HZ_CORE_TRACE("{0}", e);
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverLayer(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowClose& e)
	{
		m_Running = false;
		return true;
	}


	void Application::Run() {

		while (m_Running) {
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			float currentTime = glfwGetTime();

			m_BlueShader->Bind();
			m_SquareVA->Bind();
			glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			m_Shader->Bind();
			m_Shader->setFloat("iTime", currentTime);
			m_Shader->setVec2("iResolution", glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));
			m_VertexArray->Bind();
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			

			for (Layer* layer : m_LayerStack) 
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			//	auto [mx, my] = Input::GetMousePosition();
			//	HZ_CORE_TRACE("{0}, {1}", mx, my);

				
			m_Window->OnUpdate();

		}
	}
}