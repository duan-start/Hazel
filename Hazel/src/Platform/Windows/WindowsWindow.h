#pragma once

#include "Hazel/Core/Window.h"  // 确保 WindowProps 被定义

#include "Hazel/Renderer/GraphicsContext.h"

//前向声明
struct GLFWwindow;

namespace Hazel {
	//windowswindow的作用
	class WindowsWindow :public Window {
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate()override;

		inline uint32_t GetWidth() const override { return m_Data.Width; }
		inline uint32_t GetHeight() const override { return m_Data.Height; }
		
		//实时更改Data里面的回调函数，也就是实时更改事件处理逻辑
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
		//渲染上下文
		GraphicsContext* m_Context;
		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			//这其实是观察者模式的一个简单的实现，简单的通知（传递，1对1）
			EventCallbackFn EventCallback;
		};
		WindowData m_Data;
		//设置一个int表示窗口的个数
	};
}
