#include "EditorLayer.h"
#include <chrono>


EditorLayer::EditorLayer():
	Layer("EditorLayer"), m_CameralController(1280.f / 720.f)
{
	HZ_PROFILE_FUNCTION();

}
	
void EditorLayer::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	 m_FramebufferSize = { 1280,720 };
	Hazel::FramebufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);

	m_ActiveScene = CreateRef<Scene>();
	m_SceneHierarchyPanel.SetContext(m_ActiveScene);

	auto square = m_ActiveScene->CreateEntity("Green Square");

	square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f });
	m_SquareEntity = square;

	auto redSquare = m_ActiveScene->CreateEntity("Red Square");
	redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

	m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
	m_CameraEntity.AddComponent<CameraComponent>();

	m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");
	auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
	cc.Primary = false;

	class CameraController : public ScriptableEntity
	{
	public:
		void OnCreate()override
		{
		}

		void OnDestroy()override
		{
		}

		void OnUpdate(Timestep ts)override
		{
			auto& transform = GetComponent<TransformComponent>().Translation;
			float speed = 5.0f;

			if (Input::IsKeyPressed(HZ_KEY_A))
				transform[0] += speed * ts;
			if (Input::IsKeyPressed(HZ_KEY_D))
				transform[0] -= speed * ts;
			if (Input::IsKeyPressed(HZ_KEY_W))
				transform[1] -= speed * ts;
			if (Input::IsKeyPressed(HZ_KEY_S))
				transform[1] += speed * ts;
		}
	};

	
	m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
}

void EditorLayer::OnDetach()
{
	HZ_PROFILE_FUNCTION();

}

void EditorLayer::OnUpdate(Timestep ts)
{
	
	HZ_PROFILE_FUNCTION();


	// C++17 起可以这样用，无需指定模板参数
	//Timer myTimer("Name", []() {});

	m_CameralController.OnUpdate(ts);

	m_Framebuffer->Bind();
	Renderer2D::ResetStats();
	RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	RenderCommand::Clear();

	// Update scene
	m_ActiveScene->OnUpdate(ts);

	m_Framebuffer->Unbind();



}

void EditorLayer::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	//imgui应该自带flush
	//RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//RenderCommand::Clear();
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
	ImGui::SliderFloat("My Float", &m_Float, 0.0f, 180.0f);
	auto states = Hazel::Renderer2D::GetStats();
	//visual stats
	ImGui::Text("Renderer2D stats: ");
	ImGui::Text("DrawCalls: %d", states.DrawCalls);
	ImGui::Text("QuadCount: %d", states.QuadCount);
	ImGui::Text("QuadVertex: %d", states.GetTotalVertexCount());
	ImGui::Text("QuadIndex: %d", states.GetTotalIndexCount());

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::Begin("ViewPort");
	//Resize viewport
	ImVec2 SpaceAvil = ImGui::GetContentRegionAvail();
	if (glm::distance(m_FramebufferSize, glm::vec2(SpaceAvil.x, SpaceAvil.y)) > 1.0f && !Hazel::Input::IsMouseButtonPressed(0)) {
		m_FramebufferSize = { SpaceAvil.x, SpaceAvil.y };
		m_Framebuffer->Resize(SpaceAvil.x, SpaceAvil.y);
		m_CameralController.OnResize(SpaceAvil.x, SpaceAvil.y);
		m_ActiveScene->OnViewportResize((uint32_t)SpaceAvil.x, (uint32_t)SpaceAvil.y);
	}

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image((void*)textureID, ImVec2{ m_FramebufferSize.x, m_FramebufferSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
	ImGui::End();
	ImGui::PopStyleVar();

	m_SceneHierarchyPanel.OnImGuiRender();

}

void EditorLayer::OnEvent(Event& event)
{
	m_CameralController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressed>(
	HZ_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressed& e)
{

	if (e.GetMouseButton() == HZ_MOUSE_BUTTON_LEFT)
	{		return true; // 表示事件已处理
	}

	return false;
}
