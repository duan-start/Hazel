#pragma once

#include "Event.h"
#include <sstream>

namespace Hazel {
	class HAZEL_API MouseMovedEvent :public Event {
	public:
		MouseMovedEvent(float x, float y)
			:m_MouseX(x), m_MouseY(y) {}
		 float GetX() const { return m_MouseX; }
		 float GetY() const { return m_MouseY; }

		std::string ToString()const override {
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << "," << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

	class HAZEL_API MouseScrolledEvent :public Event {
	public:
		MouseScrolledEvent(float xOffset,float yOffset)
			:m_XOffset(xOffset),m_YOffset(yOffset){}

		 float GetXOffset(){ return m_XOffset; }
		 float GetYOffset(){ return m_YOffset; }

			std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_XOffset << "," << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;

	};

	class HAZEL_API MouseButtonEvent : public Event {
	public:
		inline int GetMouseButton() const { return m_Button; }

		//类型分类
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		//仅供子类构造
		MouseButtonEvent(int button)
			:m_Button(button){}

		int m_Button;
	};

	class HAZEL_API MouseButtonPressed : public MouseButtonEvent {
	public:
		MouseButtonPressed(int button)
			:MouseButtonEvent(button){}

	//	inline int GetButton() {return m_Button}
		std::string ToString() const override{
			std::stringstream ss;
			ss << "MouseButtonPressed: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)

	};

	class HAZEL_API MouseButtonReleased : public MouseButtonEvent {
	public:
		MouseButtonReleased(int button)
			:MouseButtonEvent(button) {}

		//	inline int GetButton() {return m_Button}
		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonReleased: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)

	};
}