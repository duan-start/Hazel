#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
Sandbox2D::Sandbox2D():
	Hazel::Layer("Sandbox2D"), m_CameralController(1280.f / 720.f)
{

}
	
void Sandbox2D::OnAttach()
{

}

void Sandbox2D::OnDettach()
{
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	m_CameralController.OnUpdate(ts);
	///---------------------------------------------
	Hazel::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	Hazel::RenderCommand::Clear();

	Hazel::Renderer2D::BeginScene(m_CameralController.GetCamera());
	Hazel::Renderer2D::DrawQurad({ 0.f,0.f,0.0f }, { 1.0f,1.0f }, m_SquareColor);
	Hazel::Renderer2D::DrawQurad({ 0.5f,0.5f }, { 1.5f,1.0f }, { 0.4f,0.2f,0.3f,0.8f });
	Hazel::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Setting");
	ImGui::ColorEdit4("Squar_colr", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event)
{
	m_CameralController.OnEvent(event);
}
