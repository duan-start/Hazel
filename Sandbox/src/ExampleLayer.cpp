#include "ExampleLayer.h"


ExampleLayer::ExampleLayer()
	:Hazel::Layer("Example"), m_CameralController(1280.f / 720.f) {


	//m_Camera->SetPosition(glm::vec3(0.5,0.5,0.5));
	//m_Camera->SetRotation(glm::vec4(0., 0., 1., 45.));

	m_VertexArray = Hazel::VertexArray::Create();

	float vertices[5 * 4] = {
		//��Ļ����     //��������
	 -0.5f,-0.5f,0.f,	0.f,0.f,//����
	 -0.5f,0.5f,0.f,	0.f,1.f,//����
	 0.5f,-0.5f,0.f,	1.f,0.f,//you xia
	 0.5f,0.5f,0.f,		1.f,1.f//you shang
	};

	m_VertexBuffer = (Hazel::VertexBuffer::Creat(vertices, sizeof(vertices)));


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
	m_IndexBuffer = (Hazel::IndexBuffer::Creat(Index, sizeof(Index) / sizeof(uint32_t)));
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);



	////�󶨵ڶ���״̬
	//m_SquareVA = (Hazel::VertexArray::Create());
	//float bluevertices[3 * 4] = {
	// -.5f,-.5f,-.5f,//����
	//-.5f,.5f,-.5f,//����
	//.5f,-.5f,-.5f,//you xia
	//.5f,.5f,-.5f//you shang
	//};

	////��Ȼ�����Ǵ�������ָ�룬����Ҳ�Ǵ����࣬Ҳ��ʵ������ע�⹹�캯��,���ǣ�û�㶮���make_shared���÷�
	//Hazel::Ref<Hazel::VertexBuffer> squareVB;
	//squareVB = (Hazel::VertexBuffer::Creat(bluevertices, sizeof(bluevertices)));

	//{//�Ѳ�Ҫ�Ķ���ȫ��������
	//	Hazel::BufferLayout layout = {
	//		//"addv"��const std::string& ���ͻ���std::string Ҳ�У�������ǧ���std::string&
	//		{Hazel::ShaderDataType::Float3, "a_Pos"},
	//	};
	//	//���û��岼��
	//	squareVB->SetLayout(layout);
	//}

	////��������
	//m_SquareVA->AddVertexBuffer(squareVB);
	//m_SquareVA->SetIndexBuffer(m_IndexBuffer);

//
//
//	//�����������Ե�
//	std::string m_BlueVertexSrc = R"(
//		#version 330 core
//		
//		// ���붥������
//		layout(location = 0) in vec3 aPos;      // ����λ��
//		uniform vec2 iResolution;
//		uniform mat4 u_ViewProjection;
//		uniform mat4 u_Transform;
//
//		// ������ɫ��������
//		void main() {
//		    // ���ö���λ��
//		    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
//		}
//)";
//	std::string m_BuleFragmentSrc = R"(
//		#version 330 core
//			uniform vec3 Color;
//			uniform float iTime;
//			out vec4 FragColor;	
//			void main(){
//			FragColor=vec4(Color,0.f);	
//			}
//)";
	//ֱ�Ӹ��ͻ���ʹ��
	//m_Shader.reset(Hazel::Shader::Create("assets/shaders/Youtube.glsl"));
	//m_BlueShader.reset(Hazel::Shader::Create("BlueShader",m_BlueVertexSrc, m_BuleFragmentSrc));

	//m_TexShader.reset(Hazel::Shader::Create("assets/shaders/Texture.glsl"));

	//���ص�Ĭ�ϵ�lib����ʹ��
	m_ShaderLib.Load("assets/shaders/Youtube.glsl");
	/*m_ShaderLib.Load("BlueShader", m_BlueVertexSrc, m_BuleFragmentSrc);*/
	//m_ShaderLib.Load("assets/shaders/Texture.glsl");

	//m_Texture = Hazel::Texture2D::Create("assets/textures/awe.png");
	//m_TextureHu = Hazel::Texture2D::Create("assets/textures/Hutao.jpg");

	//auto& m_TexShader = m_ShaderLib.Get("Texture");
	//m_TexShader->Bind();
	//�����static �����ȫ��ʵ�ʾ������Ƹ��࣬���Ը���ȫ��
	//m_TexShader->SetUniformInt("u_texture", 0);
	//m_TexShader->UnBind();
}

void ExampleLayer::OnUpdate(Hazel::Timestep ts)  {

	m_CameralController.OnUpdate(ts);
	///---------------------------------------------
	Hazel::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	Hazel::RenderCommand::Clear();

	//HZ_TRACE("{}", m_Rotation);
	//��������hack�ˣ������ߴ���shader���г�ʼ���ķ�����ֻ�뵽���m_window
	Hazel::Renderer::BeginScene(m_CameralController.GetCamera(), { 1280,720 });

	//hack
	auto& youTube = m_ShaderLib.Get("Youtube");
	youTube->Bind();
	youTube->SetUniformFloat4("color", m_SquareColor);
	youTube->UnBind();

	//������ͬʱ������װ�ŵ�render2d��������������½���ȷʵ��
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			glm::vec3 Pos(x * 0.11, y * 0.11, 0);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), Pos) * scale;
			Hazel::Renderer::Submit(youTube, m_VertexArray, transform);
		}
	}

	//����Ҳ������һ�����ٵ�hack,������shader,ʵ�����ǿ���ָ��һ�εģ���Ϊÿ��texture���԰󶨵���ͬ�Ĳ������ϡ�
	//m_TextureHu->Bind();
	//Hazel::Renderer::Submit(m_ShaderLib.Get("Texture"), m_VertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)));
	//��Ȼzbuffer�Ļ����ǿ��Բ�����˳�����ģ���������Ƿ�͸���ģ�����˳����û�й�ϵ�ġ�
	//���������͸���Ļ����ͱ���Ӻ���ǰ���ƣ�����Ϊ���Ƶ�ʱ��Ҫ���ӱ�������ɫ��
	//m_Texture->Bind();
	//Hazel::Renderer::Submit(m_TexShader, m_VertexArray);


	Hazel::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender()  {
	ImGui::Begin("Setting");
	ImGui::ColorEdit4("Squar_colr", glm::value_ptr(m_SquareColor));
	ImGui::End();
}