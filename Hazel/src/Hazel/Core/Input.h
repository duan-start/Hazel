#pragma once
#include "Hazel/Core/Core.h"

namespace Hazel {
	class HAZEL_API Input {
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsButtonPressedImpl(button); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
		inline static std::pair<float,float> GetMousePosition() { return s_Instance->GetMousePositonImpl(); }
		//virtual ~Input();
	protected:

		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsButtonPressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePositonImpl() = 0;
	private:
	 static	Input* s_Instance;
	};
}