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
			uint32_t width = 1280,
			uint32_t height = 720)
			:Title(title), Width(width), Height(height) {}
	};

	//Interface representing a desktop sustem based Window

	class HAZEL_API Window {
	public:
		//���ûص�����
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window()=default;
		virtual uint32_t GetWidth()const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void OnUpdate() = 0;  // ������һ��

		//Window attributes 
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync()const = 0;

		//����細������
		virtual void* GetNativeWindow()const = 0;

		//�����ض�ƽ̨ʵ�ֵ�
		static Window* Create(const WindowProps& props = WindowProps());
	};
}