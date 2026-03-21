#pragma once

#include"Hazel/Core/Core.h"

namespace Hazel {
	//时间在Hazel里面是一个实时堵塞的，意味着当一个事件发生时，将会立刻得到处理
	//未来，我们会设置一个更好的策略来缓冲时间在一个事件总线中，并且在”事件“这整个类型更新
	
	//枚举事件类型
	//enum class优于enum,一般只有旧版的c底层才会使用enum
	//需要显示指定命名域，不能直接隐式转换为int，必须显示static_cast<int>
	enum class EventType {
		//枚举事件类型，从而不用dynamic cast 转换不同类型，但是这个class还是具体什么
		//不同的事件类型，所有的窗口事件
		None = 0,
		WindowClose, WindowResize, WindowFocue, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	//一个事件可以属于多个category,通过位运算
	//可以隐式转int
	enum EventCategory {
		None = 0,
		//将一个事件分成多类，不局限于一个数字，考虑一个事件实际上是属于哪一个分类的
		EventCategoryApplication	 = BIT(0),
		EventCategoryInput			 = BIT(1),
		EventCategoryKeyboard		 = BIT(2),
		EventCategoryMouse			 = BIT(3),
		EventCategoryMouseButton	 = BIT(4)
	};

	//直接一个当三个用,获得事件的类型和名字
	//#type 直接把type变为const char*->"type"
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType(){ return EventType::type;}\
									virtual EventType GetEventType()const override{return GetStaticType();}\
									virtual const char* GetName() const override{return #type;}
	//放回事件的类别
	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	class HAZEL_API Event {
		friend class EventDispatcher;
	public: 
		bool Handled=false;
		//运行时多态获得准确的类型	
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags()const = 0;
		virtual std::string ToString() const { return GetName(); }

	//判断事件类型
		inline bool IsInCategory(EventCategory category) {
	//明白了，这是函数返回值和category 做位运算，从而判断事件类型是否在给定的类型中
			return GetCategoryFlags() & category;
		}
	protected:
	//判断事件是否被处理
		bool m_Handled = false;
	};

	//event事件处理
	class EventDispatcher {
	//模板定义下的函数指针，只是一个声明
		template <typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		//绑定父类实现向上转型，实现多态
		EventDispatcher(Event& event)
			:m_Event(event){}

		//虽然有函数的实参推导,但是可以通过显示的进行覆盖
		//这里的T是子类的实际事件class
		template<typename T>
		bool Dispatch(EventFn<T> func) {
			if (m_Event.Handled) return false;
			//查看事件运行时的多态是不是给定的实际的模板事件类型
			if (m_Event.GetEventType() == T::GetStaticType()) {
				//将父类指针强制转换成运行时的事件
				//fun是一个函数指针，由外部指定
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};
	//全局操作符重载
	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.ToString();
	}

}