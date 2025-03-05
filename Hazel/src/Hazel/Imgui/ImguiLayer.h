#pragma once 

#include "Hazel/Layer.h"
#include "imgui.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/keyEvent.h"
#include "Hazel/Events/ApplicationEvent.h"
namespace Hazel {
	class HAZEL_API ImguiLayer :public Layer {
	public:
		ImguiLayer();
		~ImguiLayer();
		virtual void OnAttach()override;
		virtual void OnDettach()override;
		//virtual void OnUpdate() override;
		void Begin();
		void End();
		virtual void OnImGuiRender() override;
	private:
		float m_Time=0.0f;

	};
}
