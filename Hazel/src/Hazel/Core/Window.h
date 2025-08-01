#pragma once
#include "hzpch.h"

#include "Hazel/Core/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {
	struct WindowProps {
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Hazel Engine",
			uint32_t width = 1600,
			uint32_t height = 900)
			:Title(title), Width(width), Height(height) {}
	};

	//Interface representing a desktop sustem based Window

	class HAZEL_API Window {
	public:
		//设置回调函数
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window()=default;
		virtual uint32_t GetWidth()const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void OnUpdate() = 0;  // 新增这一行

		//Window attributes 
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync()const = 0;

		//给外界窗口属性
		virtual void* GetNativeWindow()const = 0;

		//将在特定平台实现的
		static Window* Create(const WindowProps& props = WindowProps());
	};
}