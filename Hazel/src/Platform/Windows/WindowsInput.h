#pragma once
#include "Hazel/Core/Input.h"



namespace Hazel {
	class WindowsInput : public Input {

	protected:
		virtual bool IsKeyPressedImpl(int keycode) ;
		virtual bool IsButtonPressedImpl(int button) ;
		virtual float GetMouseXImpl() ;
		virtual float GetMouseYImpl() ;
		virtual std::pair<float, float> GetMousePositonImpl();
	};
}