#pragma once

    //#include"Core.h"

namespace Hazel {
	//时间在Hazel里面是一个实时堵塞的，意味着当一个事件发生时，将会立刻得到处理
	//未来，我们会设置一个更好的策略来缓冲时间在一个事件总线中，并且在”事件“这整个类型更新
	// 状态的时候再统一做处理
	enum class EventType {
		//枚举时间类型，从而不用dynamic cast 转换不同类型，但是这个class还是具体什么
		//不同的事件类型，所有的窗口事件
		None = 0,
		WindowClose, WindowResize, WindowFocue, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory {
		None = 0,
		//将一个事件分成多类，不局限于一个数字，考虑一个事件实际上是属于哪一个分类的
		EventCategoryApplication	 = BIT(0),
		EventCategoryInput			 = BIT(1),
		EventCategoryKeyboard		 = BIT(2),
		EventCategoryMouse			 = BIT(3),
		EventCategoryMouseButton	 = BIT(4)
	};

	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType(){ return EventType::type;}\
									virtual EventType GetEventType()const override{return GetStaticType();}\
									virtual const char* GetName() const override{return #type;}

	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	class HAZEL_API Event {
		friend class EventDispatcher;
	public: 
		bool Handled=false;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags()const = 0;
		virtual std::string ToString() const { return GetName(); }

	//判断事件类型
		inline bool IsInCategory(EventCategory category) {
			return GetCategoryFlags() & category;
	//明白了，这是函数返回值和category 做位运算，从而判断事件类型是否在给定的类型中
		}
	protected:
	//判断事件是否被处理
		bool m_Handled = false;
	};

	//
	class EventDispatcher {
	//函数模板加上using 替换,具体来讲，这有点像是将所有给定的参数转换成bool类型
		template <typename T>
	//这个函数的具体作用
		using EventFn = std::function<bool(T&)>;
	public:
	//Event 是一个基类
		EventDispatcher(Event& event)
			:m_Event(event){}

	//
		template<typename T>
		bool Dispatch(EventFn<T> func) {
			if (m_Event.GetEventType() == T::GetStaticType()) {
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.ToString();
	}

}