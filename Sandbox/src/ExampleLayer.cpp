#include "ExampleLayer.h"


ExampleLayer::ExampleLayer()
	:Hazel::Layer("Example"), m_CameralController(1280.f / 720.f) {


	//m_Camera->SetPosition(glm::vec3(0.5,0.5,0.5));
	//m_Camera->SetRotation(glm::vec4(0., 0., 1., 45.));

	m_VertexArray = Hazel::VertexArray::Create();

	float vertices[5 * 4] = {
		//屏幕坐标     //纹理坐标
	 -0.5f,-0.5f,0.f,	0.f,0.f,//左下
	 -0.5f,0.5f,0.f,	0.f,1.f,//左上
	 0.5f,-0.5f,0.f,	1.f,0.f,//you xia
	 0.5f,0.5f,0.f,		1.f,1.f//you shang
	};

	m_VertexBuffer = (Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));


	{//把不要的东西全部都销毁,
	//buffderlayout 实际上就是一个class ,里面封装了一个初始函数进行对应buffer里面每个BufferElements的计算，得到对应的偏移量，整体的偏移量之类的数据
	// vector<BufferElements>的数据才是这个class传递给后面的关键信息
		Hazel::BufferLayout layout = {
			//"addv"是const std::string& 类型或是std::string 也行，但就是千万别std::string&
			{ Hazel::ShaderDataType::Float3, "a_Pos"},
			{Hazel::ShaderDataType::Float2,"a_Tex"}
		};
		//设置缓冲布局，实际上这里就是传输上面计算好的各种信息
		m_VertexBuffer->SetLayout(layout);
	}

	//设置属性，这边有抽象出了一层，这里在设置vao的属性，
	// 实际上一个vao可以绑定多种状态的vbo（也就是我们的a_Pos和a_Tex），通过配置顶点属性指针和enable对应的顶点属性指针。
	//但是实际上我们除了可以写成一个数组，还可以单独拆成两个数组，vao是允许分别绑定的
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);

	//indexbuffer
	uint32_t Index[] = { 0, 1, 2,   // first triangle
	1, 2, 3    // second triangle
	};
	m_IndexBuffer = (Hazel::IndexBuffer::Creat(Index, sizeof(Index) / sizeof(uint32_t)));
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);



	////绑定第二个状态
	//m_SquareVA = (Hazel::VertexArray::Create());
	//float bluevertices[3 * 4] = {
	// -.5f,-.5f,-.5f,//左下
	//-.5f,.5f,-.5f,//左上
	//.5f,-.5f,-.5f,//you xia
	//.5f,.5f,-.5f//you shang
	//};

	////虽然这里是创建智能指针，但是也是创建类，也是实例化，注意构造函数,不是，没搞懂这个make_shared的用法
	//Hazel::Ref<Hazel::VertexBuffer> squareVB;
	//squareVB = (Hazel::VertexBuffer::Creat(bluevertices, sizeof(bluevertices)));

	//{//把不要的东西全部都销毁
	//	Hazel::BufferLayout layout = {
	//		//"addv"是const std::string& 类型或是std::string 也行，但就是千万别std::string&
	//		{Hazel::ShaderDataType::Float3, "a_Pos"},
	//	};
	//	//设置缓冲布局
	//	squareVB->SetLayout(layout);
	//}

	////设置属性
	//m_SquareVA->AddVertexBuffer(squareVB);
	//m_SquareVA->SetIndexBuffer(m_IndexBuffer);

//
//
//	//正常用来测试的
//	std::string m_BlueVertexSrc = R"(
//		#version 330 core
//		
//		// 输入顶点数据
//		layout(location = 0) in vec3 aPos;      // 顶点位置
//		uniform vec2 iResolution;
//		uniform mat4 u_ViewProjection;
//		uniform mat4 u_Transform;
//
//		// 顶点着色器主函数
//		void main() {
//		    // 设置顶点位置
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
	//直接给客户端使用
	//m_Shader.reset(Hazel::Shader::Create("assets/shaders/Youtube.glsl"));
	//m_BlueShader.reset(Hazel::Shader::Create("BlueShader",m_BlueVertexSrc, m_BuleFragmentSrc));

	//m_TexShader.reset(Hazel::Shader::Create("assets/shaders/Texture.glsl"));

	//加载到默认的lib进行使用
	m_ShaderLib.Load("assets/shaders/Youtube.glsl");
	/*m_ShaderLib.Load("BlueShader", m_BlueVertexSrc, m_BuleFragmentSrc);*/
	//m_ShaderLib.Load("assets/shaders/Texture.glsl");

	//m_Texture = Hazel::Texture2D::Create("assets/textures/awe.png");
	//m_TextureHu = Hazel::Texture2D::Create("assets/textures/Hutao.jpg");

	//auto& m_TexShader = m_ShaderLib.Get("Texture");
	//m_TexShader->Bind();
	//相对于static 会更安全（实际就是限制更多，所以更安全）
	//m_TexShader->SetUniformInt("u_texture", 0);
	//m_TexShader->UnBind();
}

void ExampleLayer::OnUpdate(Hazel::Timestep ts)  {

	m_CameralController.OnUpdate(ts);
	///---------------------------------------------
	Hazel::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	Hazel::RenderCommand::Clear();

	//HZ_TRACE("{}", m_Rotation);
	//这里我又hack了，这个宽高传给shader进行初始话的方法我只想到耦合m_window
	Hazel::Renderer::BeginScene(m_CameralController.GetCamera(), { 1280,720 });

	//hack
	auto& youTube = m_ShaderLib.Get("Youtube");
	youTube->Bind();
	youTube->SetUniformFloat4("color", m_SquareColor);
	youTube->UnBind();

	//可能是同时开启了装门的render2d，导致这边性能下降的确实大
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			glm::vec3 Pos(x * 0.11, y * 0.11, 0);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), Pos) * scale;
			Hazel::Renderer::Submit(youTube, m_VertexArray, transform);
		}
	}

	//这里也是做的一个快速的hack,绑定两次shader,实际上是可以指绑定一次的，因为每个texture可以绑定到不同的采样器上。
	//m_TextureHu->Bind();
	//Hazel::Renderer::Submit(m_ShaderLib.Get("Texture"), m_VertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)));
	//虽然zbuffer的绘制是可以不按照顺序来的，所以如果是非透明的，绘制顺序是没有关系的。
	//但是如果是透明的话，就必须从后往前绘制，（因为绘制的时候要叠加背景的颜色）
	//m_Texture->Bind();
	//Hazel::Renderer::Submit(m_TexShader, m_VertexArray);


	Hazel::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender()  {
	ImGui::Begin("Setting");
	ImGui::ColorEdit4("Squar_colr", glm::value_ptr(m_SquareColor));
	ImGui::End();
}