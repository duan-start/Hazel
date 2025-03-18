#include "hzpch.h"

#include "Application.h"
#include"Events/ApplicationEvent.h"
#include"Log.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Hazel {



	//������÷�
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Mat3:		return GL_FLOAT;
			case ShaderDataType::Mat4:		return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}
		HZ_CORE_ASSERT(false, "Unknowen ShaderDataType");
		return 0;
	}

	Application::Application() {
		HZ_CORE_ASSERT(!s_Instance, "Application has been existed ");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		//������Ҫ������������delete���ģ�
		m_ImGuiLayer = new ImguiLayer();
		PushOverlay(m_ImGuiLayer);

		//����hack����������
		glGenVertexArrays(1, &m_vertexarray);
		glBindVertexArray(m_vertexarray);
	
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.f , .8f,0.2f,.8f,1.0f,
			 0.f,  0.5f,  0.0f ,0.2f,0.3f,1.0f,1.0f,
			.5	,  -.5,   0.f,  .8f,.8f,.2f, 1.0f
		};
		m_VertexBuffer.reset(VerTexBuffer::Creat(vertices, sizeof(vertices)));
		
		//�Ѳ�Ҫ�Ķ���ȫ�������ٵ�
		{
			BufferLayout layout = {
				//"addd"��const std::string& ���ͻ���std::string Ҳ�У�������ǧ���std::string&
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float4,"a_Color"},
			};
			m_VertexBuffer->SetLayout(layout);
		}
		
		uint32_t index = 0;
		//const ��Ȼ������ǻ��bufferlayout�ĵ�����������һ��Ҫ��const���͵ķ��ص�����
		//�������begin ��end���������д�ģ���Ҫ�����ѭ�����õ������ģ���Ȼ�����˲�����Ϣ
		for (const auto& element : m_VertexBuffer->GetLayout()) {
			glEnableVertexArrayAttrib(m_vertexarray,index);
			glVertexAttribPointer(index,element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type), 
			element.Normalized ? GL_TRUE:GL_FALSE, m_VertexBuffer->GetLayout().GetStride(),
				(const void*)element.Offset);
			index++;
		}
		//indexbuffer

		uint32_t Index[] = { 0,1,2 };
		m_IndexBuffer.reset(IndexBuffer::Creat(Index, sizeof(Index) / sizeof(uint32_t)));
		

		std::string VertexSrc = R"(
		#version 330 core
		layout(location=0) in vec3 a_Pos;
		layout(location=1) in vec4 a_Color;
		out vec3 as;
		out vec4 aV;
		void main(){
		gl_Position=vec4(a_Pos,1.0f);
		as=a_Pos;
		aV=a_Color;
	}
)";
		std::string FragmentSrc = R"(
		#version 330 core
		out vec4 a_color;
		in vec3 as;
		in vec4 aV;
		void main(){
		a_color=vec4(aV);
	}
)";

		m_Shader.reset(new Shader(VertexSrc,FragmentSrc));

	}

	Application:: ~Application() {
		//����д�����ʵ����ʵ��laystack�ֶ�ɾ�������Ǹ��ط�������������
		//delete m_ImGuiLayer;  // �ֶ��ͷ� m_ImGuiLayer
	}

	//�����¼��ص�����
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
			m_Shader->Bind();
			glBindVertexArray(m_vertexarray);
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT,nullptr);

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