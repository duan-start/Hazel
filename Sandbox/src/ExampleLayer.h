#pragma once
#include "Hazel.h"


class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer();
	~ExampleLayer() = default;
	void OnEvent(Hazel::Event& e) override {

		//////HZ_TRACE("{0}", event);
		//
		//if (event.GetEventType() == Hazel::EventType::KeyPressed) {
		//	Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
		//	//if (e.GetKeyCode() == HZ_KEY_TAB)
		//	//HZ_TRACE("Tab key is pressed (event)!");
		//  //  HZ_TRACE("{0}", (char)e.GetKeyCode());
		//	e.Handled = true;
		//}
		m_CameralController.OnEvent(e);

		//if (e.GetEventType() == Hazel::EventType::WindowResize) {
		//	auto& re = (Hazel::WindowResizeEvent&)e;
		//	float zoom = (re.GetWidth()) / 1280.f;
		//	m_CameralController.SetZoomLevel(zoom);
		//}

	}
	void OnUpdate(Hazel::Timestep ts) override;

	void OnImGuiRender() override;

private:
	Hazel::ShaderLibrary m_ShaderLib;

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_TextureHu;

	//ÿ��vao ��ʾһ����Ⱦ����
	Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
	Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	//���vao
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	//std::unique_ptr<BufferLayout> m_BufferLayout;

	Hazel::OrthographicCameraController m_CameralController;

	//��Ϊ�����������ϵͳ������Ͳ���Ҫ��������������ˣ�ȫ�������ǿ�λ�ã���ʼ����
	////z��ĳ�ʼ��������ת��ʱ��ľ��Ȼ�������⣬�ͻ������Щʱ����Ⱦ֡ʧ��
	//glm::vec3 m_Position{ 0.5f,0.5f,0.0f };
	//glm::vec3 m_ModelPos;
	glm::vec4 m_SquareColor{ 0.04f,0.1f,0.11f,0.2f };
};
