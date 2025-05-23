#pragma once
#include "Hazel.h"

class Sandbox2D:public Hazel::Layer
{
public:
	Sandbox2D();
	 ~Sandbox2D()= default;

	virtual void OnAttach()override;
	virtual void OnDetach()override;
	virtual void OnUpdate(Hazel:: Timestep ts)override;
	virtual void OnImGuiRender()override;
	virtual void OnEvent(Hazel::Event& event)override;
private:
	Hazel::ShaderLibrary m_ShaderLib;
	Hazel::Ref<Hazel::Shader> m_SquareShader;
	//每个vao 表示一个渲染流程
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	//Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
	//Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
	Hazel::Ref<Hazel::Texture2D> m_Texture;

	//struct ProfileResult {
	//	const char* Name;
	//	float Time;
	//};

	//std::vector<ProfileResult> m_ProfileResults;

	Hazel::OrthographicCameraController m_CameralController;
	glm::vec4 m_SquareColor{ 0.2f,0.6f,0.8f,0.9f };
	float m_Float=45.f;
};

