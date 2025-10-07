#pragma once
#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;
namespace Hazel {
	class OpenGLContext :public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		//¼ÓÔØglad£¨openglµÄº¯Êý£©
		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_windowHandle;
	};
}