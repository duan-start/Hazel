#pragma once

#include "Hazel.h"

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
	ShaderLibrary m_ShaderLib;
	Ref<Shader> m_SquareShader;
	//每个vao 表示一个渲染流程
	Ref<VertexArray> m_SquareVA;
	//Ref<VertexBuffer> m_VertexBuffer;
	//Ref<IndexBuffer> m_IndexBuffer;
	Ref<Texture2D> m_Texture;
	Ref<Texture2D> m_BgTexture;
	Ref<Texture2D> m_SpriteSheet;
	Ref<SubTexture2D> m_sprite;
	Ref<SubTexture2D> m_sprite2;
	//struct ProfileResult {
	//	const char* Name;
	//	float Time;
	//}
	//std::vector<ProfileResult> m_ProfileResults;

	OrthographicCameraController m_CameralController;
	glm::vec4 m_SquareColor{ 0.2f,0.6f,0.8f,0.9f };
	 float m_Float=45.f;


};

