
#include "hzpch.h"

#include <Hazel.h>
#include "Platform/OpenGL/OpenGLShader.h"


class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer()
		: Layer("Example") ,m_ModelPos(0.f){
		
		m_Camera.reset(Hazel::Camera::Create(-2.0, 2.0, -2, 2));
		//m_Camera->SetPosition(glm::vec3(0.5,0.5,0.5));
		//m_Camera->SetRotation(glm::vec4(0., 0., 1., 45.));

		m_VertexArray.reset(Hazel::VerTexArray::Create());

		float vertices[5 * 4] = {
			//��Ļ����     //��������
		 -0.5f,-0.5f,0.f,	0.f,0.f,//����
		 -0.5f,0.5f,0.f,	0.f,1.f,//����
		 0.5f,-0.5f,0.f,	1.f,0.f,//you xia
		 0.5f,0.5f,0.f,		1.f,1.f//you shang
		};

		m_VertexBuffer.reset(Hazel::VerTexBuffer::Creat(vertices, sizeof(vertices)));


		{//�Ѳ�Ҫ�Ķ���ȫ��������,
		//buffderlayout ʵ���Ͼ���һ��class ,�����װ��һ����ʼ�������ж�Ӧbuffer����ÿ��BufferElements�ļ��㣬�õ���Ӧ��ƫ�����������ƫ����֮�������
		// vector<BufferElements>�����ݲ������class���ݸ�����Ĺؼ���Ϣ
			Hazel::BufferLayout layout = {
				//"addv"��const std::string& ���ͻ���std::string Ҳ�У�������ǧ���std::string&
				{ Hazel::ShaderDataType::Float3, "a_Pos"},
				{Hazel::ShaderDataType::Float2,"a_Tex"}
			};
			//���û��岼�֣�ʵ����������Ǵ����������õĸ�����Ϣ
			m_VertexBuffer->SetLayout(layout);
		}

		//�������ԣ�����г������һ�㣬����������vao�����ԣ�
		// ʵ����һ��vao���԰󶨶���״̬��vbo��Ҳ�������ǵ�a_Pos��a_Tex����ͨ�����ö�������ָ���enable��Ӧ�Ķ�������ָ�롣
		//����ʵ�������ǳ��˿���д��һ�����飬�����Ե�������������飬vao������ֱ�󶨵�
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		//indexbuffer
		uint32_t Index[] = { 0, 1, 2,   // first triangle
		1, 2, 3    // second triangle
		};
		m_IndexBuffer.reset(Hazel::IndexBuffer::Creat(Index, sizeof(Index) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);



		//�󶨵ڶ���״̬
		m_SquareVA.reset(Hazel::VerTexArray::Create());
		float bluevertices[3 * 4] = {
		 -.5f,-.5f,-.5f,//����
		-.5f,.5f,-.5f,//����
		.5f,-.5f,-.5f,//you xia
		.5f,.5f,-.5f//you shang
		};

		//��Ȼ�����Ǵ�������ָ�룬����Ҳ�Ǵ����࣬Ҳ��ʵ������ע�⹹�캯��,���ǣ�û�㶮���make_shared���÷�
		Hazel::Ref<Hazel::VerTexBuffer> squareVB;
		squareVB.reset(Hazel::VerTexBuffer::Creat(bluevertices, sizeof(bluevertices)));

		{//�Ѳ�Ҫ�Ķ���ȫ��������
			Hazel::BufferLayout layout = {
				//"addv"��const std::string& ���ͻ���std::string Ҳ�У�������ǧ���std::string&
				{Hazel::ShaderDataType::Float3, "a_Pos"},
			};
			//���û��岼��
			squareVB->SetLayout(layout);
		}

		//��������
		m_SquareVA->AddVertexBuffer(squareVB);
		m_SquareVA->SetIndexBuffer(m_IndexBuffer);

		

		//�����������Ե�
		std::string m_BlueVertexSrc = R"(
		#version 330 core
		
		// ���붥������
		layout(location = 0) in vec3 aPos;      // ����λ��
		uniform vec2 iResolution;
		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		// ������ɫ��������
		void main() {
		    // ���ö���λ��
		    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
		}
)";
		std::string m_BuleFragmentSrc = R"(
		#version 330 core
			uniform vec3 Color;
			uniform float iTime;
			out vec4 FragColor;	
			void main(){
			FragColor=vec4(Color,0.f);	
			}
)";
	
		m_Shader.reset(Hazel::Shader::Create("assets/shaders/Youtube.glsl"));
		m_BlueShader.reset(Hazel::Shader::Create(m_BlueVertexSrc, m_BuleFragmentSrc));

		m_TexShader.reset(Hazel::Shader::Create("assets/shaders/Texture.glsl"));
		m_Texture=Hazel::Texture2D::Create("assets/textures/awe.png");
		m_TextureHu= Hazel::Texture2D::Create("assets/textures/hutao.jpg");
		m_TexShader->Bind();
		//�����static �����ȫ��ʵ�ʾ������Ƹ��࣬���Ը���ȫ��
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TexShader)->UploadUniformInt("u_texture", 0);
		m_TexShader->UnBind();
	}

	void OnUpdate(Hazel::Timestep ts) override {
		
		float width  = (float)Hazel::Application::Get().GetWindow().GetWidth();
		float height = (float)Hazel::Application::Get().GetWindow().GetHeight();
		
		float aspctaio = width / height;
		m_Camera.reset(Hazel::Camera::Create(-aspctaio, aspctaio, -1, 1));

		//HZ_TRACE("{0}", ts.GetSeconds());
		if (Hazel::Input::IsKeyPressed(HZ_KEY_S)) {
			m_Position.y += m_CameraSpeed * ts;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_W)) {
			m_Position.y -= m_CameraSpeed * ts;
		}
		if (Hazel::Input::IsKeyPressed(HZ_KEY_A)) {
			m_Position.x += m_CameraSpeed * ts;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D)) {
			m_Position.x -= m_CameraSpeed * ts;
		}

		if (Hazel::Input::IsKeyPressed(HZ_KEY_E)) {
			m_Rotation += m_RotationSpeed *ts;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_Q)) {
			m_Rotation -= m_RotationSpeed *ts;
		}

		Hazel::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		Hazel::RenderCommand::Clear();

		m_Camera->SetPosition(m_Position);
		m_Camera->SetRotation(glm::vec4(0.f, 0.f, 1.f, m_Rotation));
		//HZ_TRACE("{}", m_Rotation);
		Hazel::Renderer::BeginScene(m_Camera,{width,height});

		//hack
		m_Shader->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_Shader)->UploadUniformVec3("color", m_SquareColor);
		m_Shader->UnBind();

		glm::mat4 scale = glm::scale(glm::mat4 (1.0f), glm::vec3(0.1f));
		for (int y = 0; y < 40; y++) {
			for (int x = 0; x < 40; x++) {
				glm::vec3 Pos(x * 0.11, y*0.11, 0);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), Pos) * scale;
				Hazel::Renderer::Submit(m_Shader, m_VertexArray,transform);
			}
		}

		//����Ҳ������һ�����ٵ�hack,������shader,ʵ�����ǿ���ָ��һ�εģ���Ϊÿ��texture���԰󶨵���ͬ�Ĳ������ϡ�
		m_TextureHu->Bind();
		Hazel::Renderer::Submit(m_TexShader, m_VertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)));
		//��Ȼzbuffer�Ļ����ǿ��Բ�����˳�����ģ���������Ƿ�͸���ģ�����˳����û�й�ϵ�ġ�
		//���������͸���Ļ����ͱ���Ӻ���ǰ���ƣ�����Ϊ���Ƶ�ʱ��Ҫ���ӱ�������ɫ��
		m_Texture->Bind();
		Hazel::Renderer::Submit(m_TexShader, m_VertexArray);

		
		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& event) override {

		////HZ_TRACE("{0}", event);
		
		if (event.GetEventType() == Hazel::EventType::KeyPressed) {
			Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
			//if (e.GetKeyCode() == HZ_KEY_TAB)
			//HZ_TRACE("Tab key is pressed (event)!");
		  //  HZ_TRACE("{0}", (char)e.GetKeyCode());
			e.Handled = true;
		}

	}

	void OnImGuiRender() override {
		ImGui::Begin("Setting");
		ImGui::ColorEdit3("Squar_colr", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

private:
	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::Shader> m_BlueShader, m_TexShader;

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_TextureHu;

	//ÿ��vao ��ʾһ����Ⱦ����
	Hazel::Ref<Hazel::VerTexBuffer> m_VertexBuffer;
	Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
	Hazel::Ref<Hazel::VerTexArray> m_VertexArray;

	//���vao
	Hazel::Ref<Hazel::VerTexArray> m_SquareVA;
	//std::unique_ptr<BufferLayout> m_BufferLayout;

	Hazel::Ref<Hazel::Camera> m_Camera;


	//z��ĳ�ʼ��������ת��ʱ��ľ��Ȼ�������⣬�ͻ������Щʱ����Ⱦ֡ʧ��
	glm::vec3 m_Position{ 0.5f,0.5f,0.0f };
	float m_CameraSpeed = 5.f;
	float m_RotationSpeed = 180.f;
	float m_Rotation = 0.f;
	glm::vec3 m_ModelPos;

	glm::vec3 m_SquareColor{0.04f,0.1f,0.11f};
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