#include "Sandbox2D.h"
#include <chrono>
//
//template<typename Fn>
//class Timer {
//public:
//	//转化引用，即完全不发生拷贝的关系
//	
//	//Fn 是一个模板类型参数	✅ 是
//	//形式是 Fn&&			✅ 是
//	//所以这里 Fn&& func 是一个 转发引用，并不总是右值引用。
//	Timer(const char* name,Fn&& func)
//		//这里的forward就是用来完美转发
//		:m_Name(name),m_Stop(false),m_Func(std::forward<Fn>(func))
//	{
//		m_StartTimepoint = std::chrono::high_resolution_clock::now();
//	}
//
//	void Stop(){
//		auto endTimepoint = std::chrono::high_resolution_clock::now();
//
//		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
//		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
//
//		m_Stop = true;
//		float duration = (end - start)*0.001f;
//		//std::cout << m_Name << duration << "us\n";
//		//又是一个类似回调模式的东西
//		m_Func({ m_Name,duration });
//	}
//
//	~Timer() { if (!m_Stop) Stop(); }
//private:
//	const char* m_Name;
//	bool m_Stop;
//	//专门用来计时的函数
//	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
//
//	Fn m_Func;
//};



Sandbox2D::Sandbox2D():
	Hazel::Layer("Sandbox2D"), m_CameralController(1280.f / 720.f)
{
	HZ_PROFILE_FUNCTION();

}
	
void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	//注意纹理的宽高比例和采样的比例要一致，不然会出现插值错误，结果失真
	//也有可能是纹理损坏了，用自带的图像工具重新修改（导出下）就可以正确显示了
	m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	HZ_PROFILE_FUNCTION();

}



void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	//由于这个模板的参数是构造函数里面的，所以会自动推导，不需要显示的指定
	//比如不需要Timer<decltype(lamda)>(...);
	HZ_PROFILE_FUNCTION();


	// C++17 起可以这样用，无需指定模板参数
	//Timer myTimer("Name", []() {});

		m_CameralController.OnUpdate(ts);


	{
		HZ_PROFILE_SCOPE("Render Prep ");
		///---------------------------------------------
		Hazel::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		Hazel::RenderCommand::Clear();
	}

	{
		HZ_PROFILE_SCOPE("Render Draw ");
		Hazel::Renderer2D::BeginScene(m_CameralController.GetCamera());

		Hazel::Renderer2D::DrawRotatedQuad({ -.5f,-.5f }, { 1.5f,0.5f },glm::radians(m_Float), m_SquareColor);

		Hazel::Renderer2D::DrawQuad({ 0.0f,0.0f}, { 0.5f,0.5f }, m_SquareColor);
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });

		Hazel::Renderer2D::DrawQuad({ -.5f, -.5f, -0.1f }, { 10.0f, 10.0f }, m_Texture, 10.0f);
		Hazel::Renderer2D::DrawQuad({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, m_Texture, 20.0f);
		//Hazel::Renderer2D::DrawQuad({ 0.3f,0.3f}, { 0.5f,0.5f }, m_SquareColor);
		//Hazel::Renderer2D::DrawQuad({ 0.2f,0.2f ,-0.1f}, { 5.f,5.f }, m_Texture,10.f,{0.3f,0.2f,0.2f,1.0f});

	}
	Hazel::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();
	ImGui::Begin("Setting");
	ImGui::ColorEdit4("Squar_colr", glm::value_ptr(m_SquareColor));

	ImGui::SliderFloat("My Float", &m_Float, 0.0f, 180.0f);
	//for (auto& result : m_ProfileResults) {
	//	char label[50];
	//	//添加到缓冲区
	//	strcpy(label, "%.3fms ");
	//	strcat(label, result.Name);
	//	//打印出来格式化
	//	ImGui::Text(label, result.Time);
	//}
	////每次重新更新
	//m_ProfileResults.clear();

	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event)
{
	m_CameralController.OnEvent(event);
}
