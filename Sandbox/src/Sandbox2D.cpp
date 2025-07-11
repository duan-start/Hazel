#include "Sandbox2D.h"
#include <chrono>
//
//template<typename Fn>
//class Timer {
//public:
//	//转化引用，即完全不发生拷贝的关系
//	
//	//Fn 是一个模板类型参数	✅ 是
//	//形式是 Fn&&			✅ 是
//	//所以这里 Fn&& func 是一个 转发引用，并不总是右值引用。
//	Timer(const char* name,Fn&& func)
//		//这里的forward就是用来完美转发
//		:m_Name(name),m_Stop(false),m_Func(std::forward<Fn>(func))
//	{
//		m_StartTimepoint = std::chrono::high_resolution_clock::now();
//	}
//
//	void Stop(){
//		auto endTimepoint = std::chrono::high_resolution_clock::now();
//
//		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
//		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
//
//		m_Stop = true;
//		float duration = (end - start)*0.001f;
//		//std::cout << m_Name << duration << "us\n";
//		//又是一个类似回调模式的东西
//		m_Func({ m_Name,duration });
//	}
//
//	~Timer() { if (!m_Stop) Stop(); }
//private:
//	const char* m_Name;
//	bool m_Stop;
//	//专门用来计时的函数
//	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
//
//	Fn m_Func;
//};



Sandbox2D::Sandbox2D():
	Hazel::Layer("Sandbox2D"), m_CameralController(1280.f / 720.f)
{
	HZ_PROFILE_FUNCTION();

}
	
void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	//注意纹理的宽高比例和采样的比例要一致，不然会出现插值错误，结果失真
	//也有可能是纹理损坏了，用自带的图像工具重新修改（导出下）就可以正确显示了
	m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	m_BgTexture = Hazel::Texture2D::Create("assets/textures/bg.jpg");
	m_SpriteSheet = Hazel::Texture2D::Create("assets/games/sprite/Spritesheet/mapPack_spritesheet.png");

	//Initfbo
	m_FramebufferSize = { 1280,720 };
	Hazel::FramebufferSpecification fbSpec; 
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
	m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);

	m_sprite = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2,2 }, { 63,63 });
	m_sprite2 = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7,2 }, { 63,63 });
	//Init Particles
	
	m_Particle.Position = { 0.0f, 0.0f };
	m_Particle.Velocity = { 0.0f, 0.0f }, m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeEnd = 0.0f, m_Particle.SizeVariation = 0.3f;
	m_Particle.ColorBegin = m_SquareColor;
	m_Particle.ColorEnd = { 0.6f, 0.6f, 0.6f, 1.0f };
	m_Particle.LifeTime = 4.0f;


}

void Sandbox2D::OnDetach()
{
	HZ_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	m_Particle.ColorBegin = m_SquareColor;
	//由于这个模板的参数是构造函数里面的，所以会自动推导，不需要显示的指定
	//比如不需要Timer<decltype(lamda)>(...);
	HZ_PROFILE_FUNCTION();


	// C++17 起可以这样用，无需指定模板参数
	//Timer myTimer("Name", []() {});

		m_CameralController.OnUpdate(ts);

		Hazel::Renderer2D::ResetStats();
	{
		HZ_PROFILE_SCOPE("Render Prep ");
		m_Framebuffer->Bind();
		///---------------------------------------------
		Hazel::RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		Hazel::RenderCommand::Clear();
	}

	{
		static float radiance = 0.f;
		HZ_PROFILE_SCOPE("Render Draw ");
		Hazel::Renderer2D::BeginScene(m_CameralController.GetCamera());

		Hazel::Renderer2D::DrawRotatedQuad({ .5f,-.5f }, { 1.0f,1.0f }, glm::radians(radiance), m_Texture, 5.0f, m_SquareColor);

		Hazel::Renderer2D::DrawRotatedQuad({ 0.0f,0.0f }, { 0.5f,0.5f }, glm::radians(m_Float),m_SquareColor);
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		//Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		
		//background
		Hazel::Renderer2D::DrawQuad({ 0.f, 0.f, -0.2f }, { 20.0f, 20.0f }, m_Texture, 10.0f);
		Hazel::Renderer2D::DrawQuad({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, m_Texture, 20.0f);
		Hazel::Renderer2D::EndScene();

		//new scene  //presure test
		Hazel::Renderer2D::BeginScene(m_CameralController.GetCamera());
		for (float y = -5.f; y < 5.f; y += 0.5f) {
			for (float x = -5.f; x < 5.f; x += 0.5f) {
				glm::vec2 color = { (x + 5.f) / 10.f,(y + 5.f) / 10.f };
				Hazel::Renderer2D::DrawQuad({ x, y,-0.1f }, { 0.45f, 0.45f }, glm::vec4(color.x,.4f,color.y,.5f));
			}
		}

		Hazel::Renderer2D::EndScene();
		m_Framebuffer->Unbind();

		radiance+=ts*50.f;
	}

	
#if 0
	Hazel::Renderer2D::BeginScene(m_CameralController.GetCamera());
	Hazel::Renderer2D::DrawQuad({ 0.f, 0.f, -0.2f }, { 2.0f, 2.0f }, m_sprite, 1.0f);
	Hazel::Renderer2D::DrawQuad({ 3.f, 0.f, -0.2f }, { 2.0f, 2.0f }, m_sprite2, 1.0f);
	Hazel::Renderer2D::EndScene();
#endif
	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameralController.GetCamera());
}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;


		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close"))
					Hazel::Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();


	ImGui::Begin("Setting");
	ImGui::ColorEdit4("Squar_colr", glm::value_ptr(m_SquareColor));
	ImGui::SliderFloat("My Float", &m_Float, 0.0f, 180.0f);
	auto states = Hazel::Renderer2D::GetStats();
	//visual stats
	ImGui::Text("Renderer2D stats: ");
	ImGui::Text("DrawCalls: %d", states.DrawCalls);
	ImGui::Text("QuadCount: %d", states.QuadCount);
	ImGui::Text("QuadVertex: %d", states.GetTotalVertexCount());
	ImGui::Text("QuadIndex: %d", states.GetTotalIndexCount());
	// textureID = m_BgTexture->GetRenderID();
	//ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::Begin("ViewPort");
	ImVec2 SpaceAvil= ImGui::GetContentRegionAvail();
	if (glm::distance(m_FramebufferSize, glm::vec2(SpaceAvil.x, SpaceAvil.y)) > 1.0f&&!Hazel::Input::IsMouseButtonPressed(0)) {
		m_FramebufferSize = { SpaceAvil.x, SpaceAvil.y };
		m_Framebuffer->Resize(SpaceAvil.x, SpaceAvil.y);
		m_CameralController.OnResize(SpaceAvil.x, SpaceAvil.y);
	}

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image((void*)textureID, ImVec2{ m_FramebufferSize.x, m_FramebufferSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
	ImGui::End();
	ImGui::PopStyleVar();
}

void Sandbox2D::OnEvent(Hazel::Event& event)
{
	m_CameralController.OnEvent(event);

	Hazel::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Hazel::MouseButtonPressed>(
		HZ_BIND_EVENT_FN(Sandbox2D::OnMouseButtonPressed));
}

bool Sandbox2D::OnMouseButtonPressed(Hazel::MouseButtonPressed& e)
{

	if (e.GetMouseButton() == HZ_MOUSE_BUTTON_LEFT)
	{
		auto [x, y] = Hazel::Input::GetMousePosition();
		auto width = Hazel::Application::Get().GetWindow().GetWidth();
		auto height = Hazel::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameralController.GetBounds();
		auto pos = m_CameralController.GetCamera().GetPosition();

		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x,y + pos.y };

		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);

		return true; // 表示事件已处理
	}

	return false;
}
