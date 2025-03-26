#pragma once
#include"Hazel/Core.h"
#include "Window.h"

#include"Events/Event.h"
#include"Events/ApplicationEvent.h"

#include "Hazel/Layer.h"
#include "Hazel/LayerStack.h"

#include "Hazel/Input.h"
#include "Hazel/Imgui/ImguiLayer.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/VertexArray.h"

#include "Hazel/Renderer/Camera.h"

namespace Hazel {
class HAZEL_API Application {
public:	
	Application();
	virtual ~Application();
	void Run();
	void OnEvent(Event& e);
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	inline static Application& Get() { return *s_Instance; }
	inline Window& GetWindow(){ return  *m_Window; }
private:
	bool OnWindowClose(WindowClose& e);
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
	LayerStack m_LayerStack;
	ImguiLayer* m_ImGuiLayer;


	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<Shader> m_BlueShader;

	//每个vao 表示一个渲染流程
	std::shared_ptr<VerTexBuffer> m_VertexBuffer;
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
	std::shared_ptr<VerTexArray> m_VertexArray;

	//多个vao
	std::shared_ptr<VerTexArray> m_SquareVA;
	//std::unique_ptr<BufferLayout> m_BufferLayout;

	std::shared_ptr<Camera> m_Camera;
 
private:
	static Application* s_Instance;
};
//在客户端将会被定义
Application* CreateApplication();
}
