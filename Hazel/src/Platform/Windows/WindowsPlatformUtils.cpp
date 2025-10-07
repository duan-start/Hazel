#include "hzpch.h"
#include "Hazel/Utils/PlatformUtils.h"

#include <commdlg.h>	//windows 文件对话框
#include <GLFW/glfw3.h>	
#define GLFW_EXPOSE_NATIVE_WIN32  // 允许访问 GLFW 的 Win32 原生窗口句柄
#include <GLFW/glfw3native.h>	// 提供 glfwGetWin32Window()

#include "Hazel/Core/Application.h"

namespace Hazel {

	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;	 // Windows 文件对话框配置结构体
		CHAR szFile[260] = { 0 }; //路径
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		//过滤器
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		//路径必须存在，文件必须存在，打开后不改变目录
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)//如果选中了的话，就直接放回对应的地址
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());//传递窗口，文件对话框和主窗口的声明周期一致
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

}