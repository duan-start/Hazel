#include "hzpch.h"
#include "WindowsWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include <GL/gl.h>    // Windows/Linux


#include <GLFW/glfw3.h>




namespace Hazel {
	//全局唯一资源，保证只生成一个窗口资源
	static bool s_GLFWInitialized = false;

	//简单工厂模式(不需要单独去维护一个工厂类)
	Window* Window::Create(const WindowProps& props) {
		//cpp里面放依赖和子类
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps&props) {
		HZ_PROFILE_FUNCTION();
		//创建实际的窗口
		Init(props);
	}

	WindowsWindow::~WindowsWindow() {
		HZ_PROFILE_FUNCTION();
		Shutdown();
	}
	//Core
	//创建窗口，设置回调函数
	void WindowsWindow::Init(const WindowProps& props) {
		HZ_PROFILE_FUNCTION();
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		HZ_CORE_INFO(" Creating window {0} ({1} {2})", props.Title, props.Width, props.Height);

		{
			HZ_PROFILE_SCOPE("glfwCreateWindow");
			if (!s_GLFWInitialized) {
				//glfw terminate on system shutdown;
				int success = glfwInit();
				HZ_CORE_ASSERT(success, "Could not Initialize GLFW!");
				//出现错误的回调函数
				glfwSetErrorCallback([](int error, const char* description) {
					HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
				});
				s_GLFWInitialized = true;
				}
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		}
	
		//设置初始化（为openGL配置窗口上下文）
		m_Context = new OpenGLContext(m_Window);
		//将数据中指针保存在windows（attached）,方便随时存取
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

    //实际事件由操作系统获得（glfw捕获），利用捕获的信息生成引擎端的实际事件，利用绑定的引擎端事件处理机制
		//改变窗口大小的回调函数
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
			//实际上这个事件应该是所有层栈共用的回调事件，按理渲染的层栈是专门放在渲染的ui中，但是所有的都是glfw ，所以我感觉放这里关系也不大
			glViewport(0, 0, width, height);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			//这里是我们的数据指针
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS: {
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressed event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleased event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos ) {
			WindowData& data= *(WindowData*) glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

		
	}

	void WindowsWindow::Shutdown(){
		HZ_PROFILE_FUNCTION();
		glfwDestroyWindow(m_Window);
	}
	//事件处理和swapbuffer(画面更新)
	void WindowsWindow::OnUpdate() {
		HZ_PROFILE_FUNCTION();
		glfwPollEvents();
		m_Context->SwapBuffers();
		//glfwSwapBuffers(m_Window);
		
	}

	void WindowsWindow::SetVSync(bool enabled) {
		HZ_PROFILE_FUNCTION();
		if (enabled)
			//glfw开启垂直同步的函数
			glfwSwapInterval(1);
		else
			//不设置垂直同步
			glfwSwapInterval(0);
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const {
		//像这种查询类的都没有必要设置时间测试
		return m_Data.VSync;
	}

}