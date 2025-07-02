#pragma once
#include "Hazel.h"
#include "GameTest/ParticleSystem.h"

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

	bool OnMouseButtonPressed(Hazel::MouseButtonPressed& e);
private:
	Hazel::ShaderLibrary m_ShaderLib;
	Hazel::Ref<Hazel::Shader> m_SquareShader;
	//每个vao 表示一个渲染流程
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	//Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
	//Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
	Hazel::Ref<Hazel::Texture2D> m_Texture;
	Hazel::Ref<Hazel::Texture2D> m_BgTexture;
	Hazel::Ref<Hazel::Texture2D> m_SpriteSheet;
	Hazel::Ref<Hazel::SubTexture2D> m_sprite;
	Hazel::Ref<Hazel::SubTexture2D> m_sprite2;

	Hazel::Ref<Hazel::Framebuffer> m_Framebuffer;
	//struct ProfileResult {
	//	const char* Name;
	//	float Time;
	//}
	//std::vector<ProfileResult> m_ProfileResults;

	Hazel::OrthographicCameraController m_CameralController;
	glm::vec4 m_SquareColor{ 0.2f,0.6f,0.8f,0.9f };
	 float m_Float=45.f;

	//partical system
	 ParticleProps m_Particle;
	 ParticleSystem m_ParticleSystem;


};

