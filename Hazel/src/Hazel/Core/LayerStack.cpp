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
		//emplace(pos,args...) ԭ�ع��캯�� ��insert
		//emplace_back��push_back()
		m_Layers.emplace(m_Layers.begin()+ m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverLayer(Layer* Overlayer)
	{
		//ֱ�Ӹ��������Ĳ�
		m_Layers.emplace_back(Overlayer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		//������ͨ��layer
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it!= m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}
	//����������layer
	void LayerStack::PopOverLayer(Layer* Overlayer)
	{
		//����fix��һ�£���Ȼ˵ûɶ��Ҫ�����Ǹ��淶��
		auto it = std::find(m_Layers.begin()+m_LayerInsertIndex, m_Layers.end(), Overlayer);
		if (it!= m_Layers.end())
		{
			m_Layers.erase(it);
		}
	}

}
