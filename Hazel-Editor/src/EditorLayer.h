#pragma once

#include "Hazel.h"
#include "../Panels/SceneHierarchyPanel.h"
#include "../Panels/ContentBrowserPanel.h"
#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/UniformBuffer.h"

using namespace Hazel;

class EditorLayer :public Layer
{
public:
	EditorLayer();
	~EditorLayer() = default;

	virtual void OnAttach()override;
	virtual void OnDetach()override;
	virtual void OnUpdate(Timestep ts)override;
	virtual void OnImGuiRender()override;
	virtual void OnEvent(Event& event)override;

private:
	bool OnMouseButtonPressed(MouseButtonPressed& e);
	bool OnKeyPressed(KeyPressedEvent& e);

	void OnScenePlay();
	void OnSceneStop();
	void OnSceneSimulate();
	// UI Panels
	void UI_Toolbar();

	void NewScene();
	void OpenScene();
	void OpenScene(const std::filesystem::path& path);
	void SaveScene();
	void SaveSceneAs();
	void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);
	void OnDuplicateEntity();

	void OnOverlayRender();

	// ShaderToy viewport
	void ShaderToyReload(const std::string& path);
	void ShaderToyRender(Timestep ts);
	void UI_ShaderToy();
private:
	enum class SceneState
	{
		Edit = 0, Play = 1, Simulate = 2
	};
	SceneState m_SceneState = SceneState::Edit;

	Ref<Shader> m_SquareShader;

	Ref<VertexArray> m_SquareVA;

	glm::vec2  m_FramebufferSize = { 1280,720 };
	glm::vec2 m_ViewportSize = m_FramebufferSize;
	glm::vec2 m_ViewportBounds[2];

	Ref<Framebuffer> m_Framebuffer;
	Ref<Framebuffer> m_ViewportFramebuffer;
	float m_Exposure = 1.0f;

	// ShaderToy viewport state
	bool m_ShowShaderToy = true;
	bool m_ShaderToyPlaying = true;
	Ref<Framebuffer> m_ShaderToyFramebuffer;
	Ref<Shader> m_ShaderToyShader;
	Ref<MaterialInstance> m_ShaderToyMaterial;
	Ref<UniformBuffer> m_ShaderToyUniformBuffer;
	std::string m_ShaderToyPath = "assets/shaders/ShaderToy/default.glsl";
	glm::vec2 m_ShaderToySize{ 512.0f, 512.0f };
	glm::vec2 m_ShaderToyBounds[2];
	float m_ShaderToyTime = 0.0f;
	float m_ShaderToyDelta = 0.0f;
	int m_ShaderToyFrame = 0;
	glm::vec4 m_ShaderToyMouse{ 0.0f };
	glm::vec2 m_ShaderToyMouseClick{ 0.0f };
	bool m_ShaderToyMouseDown = false;

	Ref<Scene> m_ActiveScene;
	Ref<Scene> m_EditorScene;
	Ref<Scene> m_RunTimeScene;

	std::filesystem::path m_EditorScenePath;


	Entity m_SquareEntity, m_CameraEntity, m_SecondCamera;

	bool m_PrimaryCamera = true;
	OrthographicCameraController m_CameralController;
	EditorCamera m_EditorCamera;
	Entity m_HoveredEntity = {};
	bool m_ShowPhysicsColliders = false;

	bool m_ViewportHovered;
	bool m_ViewportFocused;
	glm::vec4 m_SquareColor{ 0.2f,0.6f,0.8f,0.9f };
	 float m_Float=45.f;

	 int m_GizmoType = -1;
	 // Panels
	 SceneHierarchyPanel m_SceneHierarchyPanel;
	 ContentBrowserPanel m_ContentBrowserPanel;


	 // Editor resources
	 Ref<Texture2D> m_IconPlay, m_IconStop, m_IconSimulate;
};

