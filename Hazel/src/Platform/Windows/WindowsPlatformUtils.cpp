#include "hzpch.h"
#include "Hazel/Utils/PlatformUtils.h"

#include <commdlg.h>	//windows �ļ��Ի���
#include <GLFW/glfw3.h>	
#define GLFW_EXPOSE_NATIVE_WIN32  // ������� GLFW �� Win32 ԭ�����ھ��
#include <GLFW/glfw3native.h>	// �ṩ glfwGetWin32Window()

#include "Hazel/Core/Application.h"

namespace Hazel {

	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;	 // Windows �ļ��Ի������ýṹ��
		CHAR szFile[260] = { 0 }; //·��
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		//������
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		//·��������ڣ��ļ�������ڣ��򿪺󲻸ı�Ŀ¼
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)//���ѡ���˵Ļ�����ֱ�ӷŻض�Ӧ�ĵ�ַ
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
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());//���ݴ��ڣ��ļ��Ի���������ڵ���������һ��
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