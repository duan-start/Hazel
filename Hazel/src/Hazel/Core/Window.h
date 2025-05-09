#pragma once
#include "hzpch.h"

#include "Hazel/Core/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {
	struct WindowProps {
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Hazel Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			:Title(title), Width(width), Height(height) {}
	};

	//Interface representing a desktop sustem based Window

	class HAZEL_API Window {
	public:
		//���ûص�����
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window()=default;
		virtual unsigned int GetWidth()const = 0;
		virtual unsigned int GetHeight() const = 0;
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