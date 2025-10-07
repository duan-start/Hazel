#pragma once

#include "Hazel/Core/Window.h"  // ȷ�� WindowProps ������

#include "Hazel/Renderer/GraphicsContext.h"

//ǰ������
struct GLFWwindow;

namespace Hazel {
	//windowswindow������
	class WindowsWindow :public Window {
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate()override;

		inline uint32_t GetWidth() const override { return m_Data.Width; }
		inline uint32_t GetHeight() const override { return m_Data.Height; }
		
		//����ʱʵʱ����ʵ����һ�ֺ�����������������������¼������߼����������¼���ÿ���¼�������������������������¼���������Ϣ������hazel�˵��¼���
		virtual void SetEventCallback(const EventCallbackFn& callback) override {//
		 m_Data.EventCallback = callback;
		}
		void SetVSync(bool enabled)override;
		bool IsVSync()const override;
		inline void* GetNativeWindow() const { return m_Window; }

	private :
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;
		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
		WindowData m_Data;
		//����һ��int��ʾ���ڵĸ���
	};
}
