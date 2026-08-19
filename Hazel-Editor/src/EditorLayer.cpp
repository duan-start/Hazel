
#include "EditorLayer.h"

#include "Hazel/Scene/SceneSerializer.h"

#include "Hazel/Utils/PlatformUtils.h"

#include "ImGuizmo.h"
#include "imgui.h"
#include "Hazel/Math/Math.h"

#include <chrono>

//这行的逻辑其实是：
//using namespace Hazel; 先把 Hazel 命名空间“引入当前作用域”；
//然后你声明了一个全局变量 g_AssetPath。
//问题是：
//这个 extern 声明并没有放进 Hazel 命名空间！
//它只是当前作用域的一个“全局”声明。
//extern const std::filesystem::path g_AssetPath;
 namespace Hazel {
extern const std::filesystem::path g_AssetPath;
}

EditorLayer::EditorLayer():
	Layer("EditorLayer"), m_CameralController(1280.f / 720.f)
{
	HZ_PROFILE_FUNCTION();

}
	
//资产初始化
void EditorLayer::OnAttach()
{
	HZ_PROFILE_FUNCTION();


	//2D纹理创建
	m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
	m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");
	m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");

	//帧缓冲创建和绑定

	//参数设定
	m_FramebufferSize = { 1280,720 };
	Hazel::FramebufferSpecification fbSpec;
	//设定3个附件
	fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
	fbSpec.Width = 1280;
	fbSpec.Height = 720;

	//正式创建
	m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);

	//创建摄像机（固定）
	m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
	//创建场景并进行初始化设定
	m_ActiveScene = CreateRef<Scene>();
	
	m_ActiveScene->LoadEnvironmentMap("assets/env/Debug.tga");

	m_EditorScene = m_ActiveScene;

	//终端命令参数，允许直接打开对应的路径
	auto commandLineArgs = Application::Get().GetCommandLineArgs();
	if (commandLineArgs.Count > 1)
	{
		//进行Scene的序列化
		auto sceneFilePath = commandLineArgs[1];
		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize(sceneFilePath);
	}

	//设置当前的主要Scene
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

//Tick核心
void EditorLayer::OnUpdate(Timestep ts)
{
	
	HZ_PROFILE_FUNCTION();


	// C++17 起可以这样用，无需指定模板参数
	//Timer myTimer("Name", []() {});


	//重置统计的信息
	Renderer2D::ResetStats();

	//每次bind的时候不仅bind，还重新设置了viewport的大小
	m_Framebuffer->Bind();
	Renderer3D::SetClearColor(glm::vec4(0.0f, 0.f, 0.f, 1.0f));
	Renderer3D::Clear();

	
	 //Clear our entity ID attachment to -1 （因为默认是0的话是代表对应的entityID的）
	//是根据entityID进行选中的
	m_Framebuffer->ClearAttachment(1, -1);

	//根据不同的场景状态进行tick
	switch (m_SceneState)
	{
		//编辑器的状态，纯渲染，没有物理模拟
	case SceneState::Edit:
	{
		//if (m_ViewportFocused)
			//m_CameralController.OnUpdate(ts);
		if (m_ViewportFocused)
		m_EditorCamera.OnUpdate(ts);

		m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
		break;
	}
	//游戏的状态
	case SceneState::Play:
	{
		m_ActiveScene->OnUpdateRuntime(ts);
		break;
	}
	//模拟的状态（Editor+物理）
	case SceneState::Simulate:
	{
		if (m_ViewportFocused)
		m_EditorCamera.OnUpdate(ts);

		m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
		break;
	}
	}

	//获取鼠标在窗口的位置
	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportBounds[0].x;
	my -= m_ViewportBounds[0].y;
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	//flip(1),y轴进行反转
	my = viewportSize.y - my;
	int mouseX = (int)mx;
	int mouseY = (int)my;

	if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
	{
		//读取像素颜色
		int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
	//	HZ_CORE_WARN("Pixel data = {0}", pixelData);
		//更具读取到的ID进行实体的创建
		m_HoveredEntity = pixelData <= -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		
	}

	//可视化渲染（类似UI(Post)后处理）
	OnOverlayRender();

	m_Framebuffer->Unbind();
}

//使用Imgui正式绘制
void EditorLayer::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	// Note: Switch this to true to enable dockspace
//Copy
	//创建一个铺满整个屏幕、不可移动、没有边框的“隐形大窗口”，作为所有其他小窗口（如场景视图、属性面板）的容器
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;

//设置文件操作窗口
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

			if (ImGui::MenuItem("Exit")) Application::Get().Close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

//资产内容浏览器的更新
	m_SceneHierarchyPanel.OnImGuiRender();
	m_ContentBrowserPanel.OnImguiRenderer();

//调试信息
	{
		ImGui::Begin("Statics");
		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());
		auto states = Hazel::Renderer2D::GetStats();
		ImGui::Text("Renderer2D stats: ");
		ImGui::Text("DrawCalls: %d", states.DrawCalls);
		ImGui::Text("QuadCount: %d", states.QuadCount);
		ImGui::Text("QuadVertex: %d", states.GetTotalVertexCount());
		ImGui::Text("QuadIndex: %d", states.GetTotalIndexCount());
		ImGui::End();
	}

//物理边界
	{
		ImGui::Begin("Setting");
		ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);

		ImGui::Button("skyBox", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			//接受资产拖拽（CONTENT_BROWSER_ITEM是暗号）
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
				m_ActiveScene->LoadEnvironmentMap(texturePath.string());
				//HZ_WARN("Could not load texture {0}", texturePath.filename().string());
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}
//渲染主要视图
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::Begin("ViewPort");
	//该ui控件的一个偏移
	auto viewportOffset = ImGui::GetCursorPos(); // Includes tab bar
	//当前视口是否被点击和悬停
	m_ViewportFocused = ImGui::IsWindowFocused();
	 m_ViewportHovered = ImGui::IsWindowHovered();
	//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
	//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
	//查看当前可用的窗口剩余空间
	ImVec2 SpaceAvil = ImGui::GetContentRegionAvail();
	//作为实际的size
	m_ViewportSize = { SpaceAvil.x, SpaceAvil.y };
	//如果和已有的Frambuffer 差距过大，并且松开鼠标的前提下，进行全部的resize
	if (glm::distance(m_FramebufferSize, glm::vec2(SpaceAvil.x, SpaceAvil.y)) > 1.0f && !Hazel::Input::IsMouseButtonPressed(0)) {
		//帧缓冲的（Textre重新创建，glViewport会在绑定的时候每一帧都重新绘制）
		m_FramebufferSize = { SpaceAvil.x, SpaceAvil.y };
		m_Framebuffer->Resize(SpaceAvil.x, SpaceAvil.y);
		//Camera重新设置
		m_CameralController.OnResize(SpaceAvil.x, SpaceAvil.y);
		//Editor
		m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		//Play(游戏的Camera重新设置)
		m_ActiveScene->OnViewportResize((uint32_t)SpaceAvil.x, (uint32_t)SpaceAvil.y);
	}
	//获取TextureID
	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	//y轴flip
	ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });

	//GetBounds
	//当前窗口的大小
	auto windowSize = ImGui::GetWindowSize();
	//当前窗口左上角的实际的位置
	ImVec2 minBound = ImGui::GetWindowPos();
	//左上角
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;
	//右下角
	ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
	m_ViewportBounds[0] = { minBound.x, minBound.y };
	m_ViewportBounds[1] = { maxBound.x, maxBound.y };

	//支持场景的拖拽和序列化
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
	//获取当前选中的实体
	Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
	if (selectedEntity && m_GizmoType != -1)
	{
		//设置投影模式
		ImGuizmo::SetOrthographic(false);
		//设置当前绘制图层
		ImGuizmo::SetDrawlist();

		//当前窗口的大小
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		//设置画布
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		//在runtime绘制是没有道理的

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

		//ImGuizmo进行绘制和变换（计算，绘制和修改）
		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			//属性分解
			Math::DecomposeTransform(transform, translation, rotation, scale);

			glm::vec3 deltaRotation = rotation - tc.Rotation;
			tc.Translation = translation;
			tc.Rotation += deltaRotation;
			tc.Scale = scale;
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
	
	UI_Toolbar();
	ImGui::End();
}

//按键事件设置
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
		{
			if (shift)
				SaveSceneAs();
			else
				SaveScene();
		}

		break;
	}

	// Scene Commands
	case HZ_KEY_D:
	{
		if (control)
			OnDuplicateEntity();
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

void EditorLayer::OnScenePlay()
{

if (m_SceneState == SceneState::Simulate)
		OnSceneStop();
	
	m_SceneState = SceneState::Play;
	//active用来运行，editor用来备份
	m_ActiveScene = Scene::Copy(m_EditorScene);
	m_ActiveScene->OnRuntimeStart();

	m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnSceneStop()
{

	if (m_SceneState == SceneState::Play)
		m_ActiveScene->OnRuntimeStop();
	else if (m_SceneState == SceneState::Simulate)
		m_ActiveScene->OnSimulationStop();

	m_SceneState = SceneState::Edit;
//复原，请注意，这里是指针，所以active和edit是同一份，和play的深度拷贝不同
	m_ActiveScene = m_EditorScene;

	m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnSceneSimulate()
{
	if (m_SceneState == SceneState::Play)
		OnSceneStop();

	m_SceneState = SceneState::Simulate;

	m_ActiveScene = Scene::Copy(m_EditorScene);
	m_ActiveScene->OnSimulationStart();

	m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}
//顶部图标构建
void EditorLayer::UI_Toolbar()
{
	//元素之间以及窗口和其他之间的padding
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	//普通状态下背景为黑色（矩型）
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	//读取当前主题色，但修改透明度。
	auto& colors = ImGui::GetStyle().Colors;
	const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
	const auto& buttonActive = colors[ImGuiCol_ButtonActive];
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
	//禁用滚动条和普通的装饰的效果
	ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	
	//changed
	bool toolbarEnabled = (bool)m_ActiveScene;

	//判断当前的场景是否可用
	ImVec4 tintColor = ImVec4(1, 1, 1, 1);
	if (!toolbarEnabled)
		tintColor.w = 0.5f;


	float size = ImGui::GetWindowHeight() - 4.0f;

	//绘制第一个图标（编辑和模拟）
	{
		//引用纹理
		Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
		//设置点位
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRenderID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		{
			//如果在Edit或是Simulate的情况下点击（场景运行）
			if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
				OnScenePlay();
			//如果在paly的情况下点击（直接进行停止）
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
	}
	//编辑第二个图标（编辑和运行），在同一行下绘制
	ImGui::SameLine();
	{
		//根据状态选择图标绘制
		Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;		//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		//查询点击状态
		if (ImGui::ImageButton((ImTextureID)icon->GetRenderID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		{
			if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
				OnSceneSimulate();
			else if (m_SceneState == SceneState::Simulate)
				OnSceneStop();
		}
	}
	//弹出之前的这些设置（因为ImGui的改变是全局的）
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);
	ImGui::End();
}

void EditorLayer::NewScene()
{
	//创建新的场景
	m_ActiveScene = CreateRef<Scene>();
	m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	m_EditorScenePath = std::filesystem::path();
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
	if (m_SceneState != SceneState::Edit)
		OnSceneStop();

	if (path.extension().string() != ".hazel")
	{
		HZ_WARN("Could not load {0} - not a scene file", path.filename().string());
		return;
	}
	//创建新场景
	Ref<Scene> newScene = CreateRef<Scene>();
	//进行序列化数据读取
	SceneSerializer serializer(newScene);
	if (serializer.Deserialize(path.string()))
	{
		m_EditorScene = newScene;
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);
		//这里是指针，
		m_ActiveScene = m_EditorScene;
		//保存地址（能够实时保存回去）
		m_EditorScenePath = path;
	}
}

//保存场景（如果场景path非空的话）
void EditorLayer::SaveScene()
{
	if (!m_EditorScenePath.empty())
		SerializeScene(m_ActiveScene, m_EditorScenePath);
	else
		SaveSceneAs();
}

//打开面板，创建新文件进行保存
void EditorLayer::SaveSceneAs()
{
	std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
	if (!filepath.empty())
	{
		SerializeScene(m_ActiveScene, filepath);
		m_EditorScenePath = filepath;
	}
}

//实现数据序列化
void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
{
	SceneSerializer serializer(scene);
	serializer.Serialize(path.string());
}

void EditorLayer::OnDuplicateEntity()
{
	if (m_SceneState != SceneState::Edit)
		return;

	Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
	if (selectedEntity)
		m_EditorScene->DuplicateEntity(selectedEntity);
}

void EditorLayer::OnOverlayRender()
{
	//设置相机状态（对齐绘制）
	if (m_SceneState == SceneState::Play)
	{
		Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
		if (!camera)
			return;
		Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
	}
	else
	{
		Renderer2D::BeginScene(m_EditorCamera);
	}

	if (m_ShowPhysicsColliders)
	{
		// 获取Box物理组件对应的实体ID
		{
			//拥有这两个组件的实体
			auto group = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();

			for (auto entity : group)
			{
				//实际的Component的获取
				auto [tc, bc2d] = group.get<TransformComponent, BoxCollider2DComponent>(entity);

				//向近出偏移（防止深度的问题）
				glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
				//进行x,y,z的缩放
				glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

				//经典srt方式
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), scale);

				//绘制没有EntityId属性的显示框
				Renderer2D::DrawRect(transform, glm::vec4(1, 0.5, 0, 1));
			}
		}

		// 获取Circle物理组件对应的实体ID
		{
			auto group = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
			for (auto entity : group)
			{
				auto [tc, cc2d] = group.get<TransformComponent, CircleCollider2DComponent>(entity);

				glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
				glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawCircle(transform, glm::vec4(0, 0, 1, 1), 0.02f);
			}
		}
	}

	Renderer2D::EndScene();
}

//事件处理
void EditorLayer::OnEvent(Event& event)
{
	if (m_ViewportHovered) {
		m_CameralController.OnEvent(event);
		m_EditorCamera.OnEvent(event);
	}
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
