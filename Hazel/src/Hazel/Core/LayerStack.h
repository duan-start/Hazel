#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Core/Layer.h"

namespace Hazel {
	class HAZEL_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverLayer(Layer* Overlayer);
		void PopLayer(Layer* layer);
		void PopOverLayer(Layer* Overlayer);

		//�򵥵�����һ����װ
		inline std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		inline std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		// ����������������������ͣ�
		inline std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		inline std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

	private:
		//��������
		std::vector<Layer*> m_Layers;
		//��ͨlayer��overlayer�ĵ�index
		unsigned int m_LayerInsertIndex=0;

	};
}


