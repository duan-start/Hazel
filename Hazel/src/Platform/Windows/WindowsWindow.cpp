#include "hzpch.h"
#include "WindowsWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include <GL/gl.h>    // Windows/Linux


#include <GLFW/glfw3.h>




namespace Hazel {
	static bool s_GLFWInitialized = false;

	//������ģʽ�ǹ���ģʽ
	Window* Window::Create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps&props) {
		HZ_PROFILE_FUNCTION();
		Init(props);
	}

	WindowsWindow::~WindowsWindow() {
		HZ_PROFILE_FUNCTION();
		Shutdown();
	}

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
				glfwSetErrorCallback([](int error, const char* description) {
					HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
				});
				s_GLFWInitialized = true;
				}
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		}
	
		//������Ը�һ�½ӿڵ���
		m_Context = new OpenGLContext(m_Window);
		//��ͻȻ�������Ѿ�������ӵ��˳�ʼ������ m_Context->Init();
		//���������Ķ����ŵ�����Ľӿ�
		//glfwMakeContextCurrent(m_Window);
		//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		////ʹ�ö�������Ƿ���سɹ�
		//HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
		
		//�����ǵ�����ָ�봫�ݸ�glfw
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

    //����glfw����Ļص������������Լ����¼�������
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
	
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
			//ʵ��������¼�Ӧ�������в�ջ���õĻص��¼���������Ⱦ�Ĳ�ջ��ר�ŷ�����Ⱦ��ui�У��������еĶ���glfw �������Ҹо��������ϵҲ����
			glViewport(0, 0, width, height);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			//���������ǵ�����ָ��
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
	void WindowsWindow::OnUpdate() {
		HZ_PROFILE_FUNCTION();
		glfwPollEvents();
		m_Context->SwapBuffers();
		//glfwSwapBuffers(m_Window);
		
	}

	void WindowsWindow::SetVSync(bool enabled) {
		HZ_PROFILE_FUNCTION();
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const {
		//�����ֲ�ѯ��Ķ�û�б�Ҫ����ʱ�����
		return m_Data.VSync;
	}

}