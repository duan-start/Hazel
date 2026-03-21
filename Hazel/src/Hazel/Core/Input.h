#pragma once
#include "Hazel/Core/Core.h"

namespace Hazel {
	class HAZEL_API Input {
	public:
		//静态全局接口
		//这也是一种比较好的设计方式，实现了static函数的多态实现
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsButtonPressedImpl(button); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
		inline static std::pair<float,float> GetMousePosition() { return s_Instance->GetMousePositonImpl(); }
		//virtual ~Input();
	protected:
		//子类实现多态，平台抽象
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsButtonPressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePositonImpl() = 0;
	private:
	 static	Input* s_Instance;
	};
}