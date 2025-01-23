#pragma once
#include"Hazel/Core.h"
#include"Events/Event.h"
#include "Window.h"

namespace Hazel {
class HAZEL_API Application {
public:	
	Application();
	virtual ~Application();
	void Run();
private:
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
};
//在客户端将会被定义
Application* CreateApplication();
}
