#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Core/Timestep.h"
namespace Hazel {
	//抽象基类，接口
	class HAZEL_API  Layer
	{
	public:
		Layer(const std::string& debug_name = "Layer");
		virtual ~Layer();
		//实际上是这个层的资源的一个初始化
		virtual void OnAttach();
		virtual void OnDetach();
		//更新游戏逻辑、物理、动画
		virtual void OnUpdate(Timestep ts);
		//绘制调试界面 / 编辑器界面
		virtual void OnImGuiRender();
		//事件处理
		virtual void OnEvent(Event& event);
		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};


}


