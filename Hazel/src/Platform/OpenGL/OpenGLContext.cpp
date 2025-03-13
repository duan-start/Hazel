#include "hzpch.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "OpenGLContext.h"


namespace Hazel {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle):
		m_windowHandle(windowHandle)
	{		
		Init();
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		//ʹ�ö�������Ƿ���سɹ�
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");

		//�ռ��Կ���Ϣ
		HZ_CORE_INFO("HZ_CORE_INFO:");
		HZ_CORE_INFO("OpenGL VENDOR:{0}",glGetString(GL_VENDOR));
		HZ_CORE_INFO("OpenGL VERSION:{0}",glGetString(GL_VERSION));
		HZ_CORE_INFO("OpenGL :");
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}