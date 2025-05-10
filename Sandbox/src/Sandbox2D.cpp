#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
Sandbox2D::Sandbox2D():
	Hazel::Layer("Sandbox2D"), m_CameralController(1280.f / 720.f)
{


}
	

void Sandbox2D::OnAttach()
{
	m_SquareVA = (Hazel::VerTexArray::Create());

	float vertices[4 * 3] = {
		//屏幕坐标     
		-.5f,-.5f,-.5f,//左下
		-.5f,.5f,-.5f,//左上
		.5f,-.5f,-.5f,//you xia
		.5f,.5f,-.5f//you shang
	};

	m_VertexBuffer = Hazel::VerTexBuffer::Creat(vertices, sizeof(vertices));

	{
		Hazel::BufferLayout layout = {

			{ Hazel::ShaderDataType::Float3, "a_Pos"}
		};

		m_VertexBuffer->SetLayout(layout);
	}


	//indexbuffer
	uint32_t Index[] = { 0, 1, 2,   // first triangle
	1, 2, 3    // second triangle
	};
	m_IndexBuffer.reset(Hazel::IndexBuffer::Creat(Index, sizeof(Index) / sizeof(uint32_t)));

	m_SquareVA->AddVertexBuffer(m_VertexBuffer);
	m_SquareVA->SetIndexBuffer(m_IndexBuffer);

	//正常用来测试的
	std::string m_BlueVer = R"(
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
)";
	std::string m_BluePix = R"(
		#version 330 core
			uniform vec4 Color;
			out vec4 FragColor;	
			void main(){
			FragColor=Color;	
			}
)";
	m_SquareShader = (Hazel::Shader::Create("BlueShader", m_BlueVer, m_BluePix));

}

void Sandbox2D::OnDettach()
{
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	m_CameralController.OnUpdate(ts);
	///---------------------------------------------
	
		//auto& m_SquareShader = m_ShaderLib.Get("BlueShader");
		m_SquareShader->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_SquareShader)->UploadUniformVec4("Color", m_SquareColor);
		m_SquareShader->UnBind();


	Hazel::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	Hazel::RenderCommand::Clear();

	Hazel::Renderer::BeginScene(m_CameralController.GetCamera(), { 1280,720 });

	Hazel::Renderer::Submit(m_SquareShader,m_SquareVA);

	Hazel::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Setting");
	ImGui::ColorEdit4("Squar_colr", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event)
{
	m_CameralController.OnEvent(event);
}
