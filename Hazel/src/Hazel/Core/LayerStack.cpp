#include "hzpch.h"
#include "LayerStack.h"

namespace Hazel {
	LayerStack::LayerStack()
	{
		
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		//emplace(pos,args...) 原地构造函数 和insert
		//emplace_back和push_back()
		m_Layers.emplace(m_Layers.begin()+ m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverLayer(Layer* Overlayer)
	{
		//直接覆盖最后面的层
		m_Layers.emplace_back(Overlayer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		//弹出普通的layer
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it!= m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}
	//弹出最后面的layer
	void LayerStack::PopOverLayer(Layer* Overlayer)
	{
		//这里fix了一下，虽然说没啥必要，但是更规范了
		auto it = std::find(m_Layers.begin()+m_LayerInsertIndex, m_Layers.end(), Overlayer);
		if (it!= m_Layers.end())
		{
			m_Layers.erase(it);
		}
	}

}
