#pragma once
#include"Core.h"
#include"Events/Event.h"
#include"Events/ApplicationEvent.h"
#include "Window.h"

namespace Hazel {
class HAZEL_API Application {
public:	
	Application();
	virtual ~Application();
	void OnEvent(Event&);
	void Run();
private:
	bool OnWindowClose(WindowCloseEvent&e);
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
};
//在客户端将会被定义
Application* CreateApplication();
}
