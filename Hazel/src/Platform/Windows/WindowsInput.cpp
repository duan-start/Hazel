
#include "hzpch.h"
#include "WindowsInput.h"
#include "Hazel/Core/Application.h"
#include <GLFW/glfw3.h>


namespace Hazel {
	Input* Input::s_Instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode)
{
  	auto m_Window = static_cast<GLFWwindow*> (Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(m_Window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsButtonPressedImpl(int button)
{
	auto m_Window = static_cast<GLFWwindow*> (Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetMouseButton(m_Window, button);
	return state == GLFW_PRESS;
}

float WindowsInput::GetMouseXImpl()
{
	auto [x, y] = GetMousePositonImpl();
	return (float)x;
}

float WindowsInput::GetMouseYImpl()
{
	auto [x, y] = GetMousePositonImpl(); 

	return (float)y;
}

std::pair<float, float> WindowsInput::GetMousePositonImpl()
{

	auto m_Window = static_cast<GLFWwindow*> (Application::Get().GetWindow().GetNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(m_Window, &xpos, &ypos);
	
	return { (float)xpos, (float)ypos };

}

}


