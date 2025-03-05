#pragma once
#include "Hazel/Core.h"
#include "Hazel/Layer.h"

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

		inline std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		inline std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex=0;

	};
}


