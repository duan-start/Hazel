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
		//使用断言完成是否加载成功
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");

		//收集显卡信息
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