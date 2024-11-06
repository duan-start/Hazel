#include "hzpch.h"
#include "WindowsWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/KeyEvent.h"
//为什么没有包含基类的events，因为window已经包含了基类事件，因为在实现的文件中才需要对具体细节的引用

namespace Hazel {
	static bool s_GLFWInitialized = false;//静态变量，防止反复对此创建

	Window* Window::Create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		Init(props);
	}

	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}
	void WindowsWindow::Init(const WindowProps& props)
	 {
	
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		HZ_CORE_INFO(" Creating window {0} ({1} {2})", props.Title, props.Width, props.Height);


		if (!s_GLFWInitialized) {
			//glfw terminate on system shutdown;
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not Initialize GLFW!");
			glfwSetErrorCallback([](int error, const char* description) {
				HZ_CORE_ERROR("GLFW ERROR ({0}):{1}", error, description);
				});
			s_GLFWInitialized = true;
		}
		//主要是glfw 函数的调用，用于真正的创建一个窗口，然后基于这个窗口进行相应的创建
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);//设置垂直同步
		//设置glfw 的函数回调
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data= *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			WindowResizeEvent event(width, height);
			data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window,int key,int scancode,int action,int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action) {
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);//每次调用根据不同的event去回调不同的函数
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}

			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);//这里面的事件怎么具体没有连接到的？头文件少了个.h
				data.EventCallback(event);
				break;
			}
			}
			});

			glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action,int mods) {
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action) {
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);//调用具体的event,而没有返回值，表明它实际上是一个回调给application ，起传递的作用，不同层；
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
				});
			glfwSetScrollCallback(m_Window, [](GLFWwindow* window,double xOffset,double yOffset) {
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
				});

			glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {//这里回调有点没明白，函数的参数不应该是double 吗？
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);

				});
	}

			
		

		

		void WindowsWindow::Shutdown(){
		glfwDestroyWindow(m_Window);
	}
	void WindowsWindow::OnUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled) {
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const {
		return m_Data.VSync;
	}

}