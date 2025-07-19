#pragma once

#include "Hazel.h"
#include "../Panels/SceneHierarchyPanel.h"

using namespace Hazel;

class EditorLayer:public Layer
{
public:
	EditorLayer();
	 ~EditorLayer()= default;

	virtual void OnAttach()override;
	virtual void OnDetach()override;
	virtual void OnUpdate( Timestep ts)override;
	virtual void OnImGuiRender()override;
	virtual void OnEvent(Event& event)override;

	bool OnMouseButtonPressed(MouseButtonPressed& e);
private:
	Ref<Shader> m_SquareShader;

	Ref<VertexArray> m_SquareVA;

	glm::vec2  m_FramebufferSize = { 1280,720 };

	Ref<Framebuffer> m_Framebuffer;

	Ref<Scene> m_ActiveScene;
	Entity m_SquareEntity, m_CameraEntity, m_SecondCamera;
	bool m_PrimaryCamera = true;
	OrthographicCameraController m_CameralController;

	glm::vec4 m_SquareColor{ 0.2f,0.6f,0.8f,0.9f };
	 float m_Float=45.f;

	 // Panels
	 SceneHierarchyPanel m_SceneHierarchyPanel;
};

