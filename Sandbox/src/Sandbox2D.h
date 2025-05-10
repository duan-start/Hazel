#pragma once
#include "Hazel.h"

class Sandbox2D:public Hazel::Layer
{
public:
	Sandbox2D();
	 ~Sandbox2D()= default;

	virtual void OnAttach()override;
	virtual void OnDettach()override;
	virtual void OnUpdate(Hazel:: Timestep ts)override;
	virtual void OnImGuiRender()override;
	virtual void OnEvent(Hazel::Event& event)override;
private:
	Hazel::ShaderLibrary m_ShaderLib;
	Hazel::Ref<Hazel::Shader> m_SquareShader;
	//每个vao 表示一个渲染流程
	Hazel::Ref<Hazel::VerTexArray> m_SquareVA;
	Hazel::Ref<Hazel::VerTexBuffer> m_VertexBuffer;
	Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;


	Hazel::OrthographicCameraController m_CameralController;
	glm::vec4 m_SquareColor{ 0.04f,0.1f,0.11f,0.5f };
};

