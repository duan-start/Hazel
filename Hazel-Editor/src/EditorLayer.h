#pragma once

#include "Hazel.h"
#include "../Panels/SceneHierarchyPanel.h"

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

	void NewScene();
	void OpenScene();
	void SaveSceneAs();
private:
	Ref<Shader> m_SquareShader;

	Ref<VertexArray> m_SquareVA;

	glm::vec2  m_FramebufferSize = { 1280,720 };
	glm::vec2 m_ViewportSize = m_FramebufferSize;

	Ref<Framebuffer> m_Framebuffer;

	Ref<Scene> m_ActiveScene;
	Entity m_SquareEntity, m_CameraEntity, m_SecondCamera;
	bool m_PrimaryCamera = true;
	OrthographicCameraController m_CameralController;

	glm::vec4 m_SquareColor{ 0.2f,0.6f,0.8f,0.9f };
	 float m_Float=45.f;

	 int m_GizmoType = -1;
	 // Panels
	 SceneHierarchyPanel m_SceneHierarchyPanel;
};

