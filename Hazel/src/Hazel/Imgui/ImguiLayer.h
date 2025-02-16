#pragma once 

#include "Hazel/Layer.h"

namespace Hazel {
	class HAZEL_API ImguiLayer :public Layer {
	public:
		ImguiLayer();
		~ImguiLayer();
		 void OnAttach();
		 void OnDettach();
		 void OnUpdate() override;
	void OnEvent(Event& event) override;
	private:
		float m_Time=0.0f;

	};
}
