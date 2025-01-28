#include "hzpch.h"

#include "Application.h"
#include"Events/ApplicationEvent.h"
#include"Log.h"

#include <GLFW/glfw3.h>

namespace Hazel {

	//这个的用法
	#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
	}

	Application:: ~Application() {

	}

	//设置事件回调函数
	void Application::OnEvent(Event& e) {
		HZ_CORE_TRACE("{0}", e);
	}
	void Application::Run() {
		
		while (m_Running) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}

	}
}