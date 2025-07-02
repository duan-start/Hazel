#include "EditorLayer.h"
#include <chrono>


EditorLayer::EditorLayer():
	Layer("EditorLayer"), m_CameralController(1280.f / 720.f)
{
	HZ_PROFILE_FUNCTION();

}
	
void EditorLayer::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	//注意纹理的宽高比例和采样的比例要一致，不然会出现插值错误，结果失真
	//也有可能是纹理损坏了，用自带的图像工具重新修改（导出下）就可以正确显示了
	m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");
	m_BgTexture = Texture2D::Create("assets/textures/bg.jpg");
	m_SpriteSheet = Texture2D::Create("assets/games/sprite/Spritesheet/mapPack_spritesheet.png");


	m_sprite = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2,2 }, { 63,63 });
	m_sprite2 = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7,2 }, { 63,63 });
	//Init Particles

}

void EditorLayer::OnDetach()
{
	HZ_PROFILE_FUNCTION();

}

void EditorLayer::OnUpdate(Timestep ts)
{
	
	HZ_PROFILE_FUNCTION();


	// C++17 起可以这样用，无需指定模板参数
	//Timer myTimer("Name", []() {});

		m_CameralController.OnUpdate(ts);

		Renderer2D::ResetStats();
	{
		HZ_PROFILE_SCOPE("Render Prep ");
		///---------------------------------------------
		RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		RenderCommand::Clear();
	}

	{
		static float radiance = 0.f;
		HZ_PROFILE_SCOPE("Render Draw ");
		Renderer2D::BeginScene(m_CameralController.GetCamera());

		Renderer2D::DrawRotatedQuad({ .5f,-.5f }, { 1.0f,1.0f }, glm::radians(radiance), m_Texture, 5.0f, m_SquareColor);

		Renderer2D::DrawRotatedQuad({ 0.0f,0.0f }, { 0.5f,0.5f }, glm::radians(m_Float),m_SquareColor);
		Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		//Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		
		//background
		Renderer2D::DrawQuad({ 0.f, 0.f, -0.2f }, { 20.0f, 20.0f }, m_Texture, 10.0f);
		Renderer2D::DrawQuad({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, m_Texture, 20.0f);
		Renderer2D::EndScene();

		//new scene  //presure test
		Renderer2D::BeginScene(m_CameralController.GetCamera());
		for (float y = -5.f; y < 5.f; y += 0.5f) {
			for (float x = -5.f; x < 5.f; x += 0.5f) {
				glm::vec2 color = { (x + 5.f) / 10.f,(y + 5.f) / 10.f };
				Renderer2D::DrawQuad({ x, y,-0.1f }, { 0.45f, 0.45f }, glm::vec4(color.x,.4f,color.y,.5f));
			}
		}

		Renderer2D::EndScene();

		radiance+=ts*50.f;
	}

	
#if 0
	Renderer2D::BeginScene(m_CameralController.GetCamera());
	Renderer2D::DrawQuad({ 0.f, 0.f, -0.2f }, { 2.0f, 2.0f }, m_sprite, 1.0f);
	Renderer2D::DrawQuad({ 3.f, 0.f, -0.2f }, { 2.0f, 2.0f }, m_sprite2, 1.0f);
	Renderer2D::EndScene();
#endif

}

void EditorLayer::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();


	ImGui::Begin("Setting");
	ImGui::ColorEdit4("Squar_colr", glm::value_ptr(m_SquareColor));
	ImGui::SliderFloat("My Float", &m_Float, 0.0f, 180.0f);
	auto states = Renderer2D::GetStats();
	//visual stats
	ImGui::Text("Renderer2D stats: ");
	ImGui::Text("DrawCalls: %d", states.DrawCalls);
	ImGui::Text("QuadCount: %d", states.QuadCount);
	ImGui::Text("QuadVertex: %d", states.GetTotalVertexCount());
	ImGui::Text("QuadIndex: %d", states.GetTotalIndexCount());
	uint32_t textureID = m_BgTexture->GetRenderID();
	ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });

	ImGui::End();
}

void EditorLayer::OnEvent(Event& event)
{
	m_CameralController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressed>(
		HZ_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressed& e)
{

	if (e.GetMouseButton() == HZ_MOUSE_BUTTON_LEFT)
	{		return true; // 表示事件已处理
	}

	return false;
}
