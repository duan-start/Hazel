
#include "EditorLayer.h"

#include "Hazel/Scene/SceneSerializer.h"

#include "Hazel/Utils/PlatformUtils.h"

#include "ImGuizmo.h"
#include "imgui.h"
#include "Hazel/Math/Math.h"

#include <chrono>

namespace Hazel {
extern const std::filesystem::path g_AssetPath;
}

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
	fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);

	m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
	m_ActiveScene = CreateRef<Scene>();

	auto commandLineArgs = Application::Get().GetCommandLineArgs();
	if (commandLineArgs.Count > 1)
	{
		auto sceneFilePath = commandLineArgs[1];
		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize(sceneFilePath);
	}

	m_SceneHierarchyPanel.SetContext(m_ActiveScene);

#if 0
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
#endif
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
	m_EditorCamera.OnUpdate(ts);


	//Render
	Renderer2D::ResetStats();


	m_Framebuffer->Bind();
	RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	RenderCommand::Clear();
	 //Clear our entity ID attachment to -1
	m_Framebuffer->ClearAttachment(1, -1);

	// Update scene
	//m_ActiveScene->OnUpdateRuntime(ts);
	m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportBounds[0].x;
	my -= m_ViewportBounds[0].y;
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	my = viewportSize.y - my;
	int mouseX = (int)mx;
	int mouseY = (int)my;

	if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
	{
		int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
	//	HZ_CORE_WARN("Pixel data = {0}", pixelData);
		m_HoveredEntity = pixelData <= -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		
	}

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

	//有一个初始化的ui大小
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;

	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{


			if (ImGui::MenuItem("New", "Ctrl+N"))
				NewScene();

			if (ImGui::MenuItem("Open...", "Ctrl+O"))
				OpenScene();

			if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				SaveSceneAs();

			if (ImGui::MenuItem("Close"))
				Hazel::Application::Get().Close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();


	ImGui::Begin("Setting");

	std::string name = "None";
	if (m_HoveredEntity)
		name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
	ImGui::Text("Hovered Entity: %s", name.c_str());

	//ImGui::SliderFloat("My Float", &m_Float, 0.0f, 180.0f);
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

	auto viewportOffset = ImGui::GetCursorPos(); // Includes tab bar
	//Resize viewport

	//这段代码我需要重新看一遍
	auto m_ViewportFocused = ImGui::IsWindowFocused();
	 m_ViewportHovered = ImGui::IsWindowHovered();
	//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
	//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);



	//如果界面大小更改，全部重新resize一下
	ImVec2 SpaceAvil = ImGui::GetContentRegionAvail();
	m_ViewportSize = { SpaceAvil.x, SpaceAvil.y };
	if (glm::distance(m_FramebufferSize, glm::vec2(SpaceAvil.x, SpaceAvil.y)) > 1.0f && !Hazel::Input::IsMouseButtonPressed(0)) {
		m_FramebufferSize = { SpaceAvil.x, SpaceAvil.y };
		m_Framebuffer->Resize(SpaceAvil.x, SpaceAvil.y);
		m_CameralController.OnResize(SpaceAvil.x, SpaceAvil.y);
		m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		m_ActiveScene->OnViewportResize((uint32_t)SpaceAvil.x, (uint32_t)SpaceAvil.y);
	}

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });

	//GetBounds
	auto windowSize = ImGui::GetWindowSize();
	ImVec2 minBound = ImGui::GetWindowPos();
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;

	ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
	m_ViewportBounds[0] = { minBound.x, minBound.y };
	m_ViewportBounds[1] = { maxBound.x, maxBound.y };

	//都是在这个imgui的viewport窗口里面的数据
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			OpenScene(std::filesystem::path(g_AssetPath) / path);
		}
		ImGui::EndDragDropTarget();
	}

	// Gizmos   
	Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
	if (selectedEntity && m_GizmoType != -1)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

		// Runtime Camera
		/*auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
		const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
		const glm::mat4& cameraProjection = camera.GetProjection();
		glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());*/
		// Editor camera
		const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
		glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();


		// Entity transform
		auto& tc = selectedEntity.GetComponent<TransformComponent>();
		glm::mat4 transform = tc.GetTransform();

		// Snapping
		bool snap = Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL);
		float snapValue = 0.5f; // Snap to 0.5m for translation/scale
		// Snap to 45 degrees for rotation
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			snapValue = 45.0f;

		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);

			glm::vec3 deltaRotation = rotation - tc.Rotation;
			tc.Translation = translation;
			tc.Rotation += deltaRotation;
			tc.Scale = scale;
		}
	}


	ImGui::End();
	ImGui::PopStyleVar();

	m_SceneHierarchyPanel.OnImGuiRender();
	m_ContentBrowserPanel.OnImguiRenderer();
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
{
	// Shortcuts
	if (e.GetRepeatCount() > 0)
		return false;

	bool control = Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL) || Input::IsKeyPressed(HZ_KEY_RIGHT_CONTROL);
	bool shift = Input::IsKeyPressed(HZ_KEY_LEFT_SHIFT) || Input::IsKeyPressed(HZ_KEY_RIGHT_SHIFT);
	switch (e.GetKeyCode())
	{
	case HZ_KEY_N:
	{
		if (control)
			NewScene();

		break;
	}
	case HZ_KEY_O:
	{
		if (control)
			OpenScene();

		break;
	}
	case HZ_KEY_S:
	{
		if (control)
			SaveSceneAs();

		break;
	}

	// Gizmos
	case HZ_KEY_Q:
		m_GizmoType = -1;
		break;
	case HZ_KEY_M:
		m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		break;
	case HZ_KEY_R:
		m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		break;
	case HZ_KEY_Z:
		m_GizmoType = ImGuizmo::OPERATION::SCALE;
		break;

	}
	return false;
}

void EditorLayer::NewScene()
{
	m_ActiveScene = CreateRef<Scene>();
	m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OpenScene()
{
	std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");
	if (!filepath.empty())
	{
		OpenScene(filepath);
	}
}

void EditorLayer::OpenScene(const std::filesystem::path& path)
{
	m_ActiveScene = CreateRef<Scene>();
	m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	m_SceneHierarchyPanel.SetContext(m_ActiveScene);

	SceneSerializer serializer(m_ActiveScene);
	serializer.Deserialize(path.string());
}

void EditorLayer::SaveSceneAs()
{
	std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
	if (!filepath.empty())
	{
		SceneSerializer serializer(m_ActiveScene);
		serializer.Serialize(filepath);
	}
}


void EditorLayer::OnEvent(Event& event)
{
	m_CameralController.OnEvent(event);
	m_EditorCamera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressed>(HZ_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	dispatcher.Dispatch<MouseButtonPressed>(HZ_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressed& e)
{

	if (e.GetMouseButton() == HZ_MOUSE_BUTTON_LEFT)
	{
		if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(HZ_KEY_LEFT_ALT))
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
	}
	return false;
}
