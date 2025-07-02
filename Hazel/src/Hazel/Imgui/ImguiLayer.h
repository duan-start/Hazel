#pragma once 

#include "Hazel/Core/Layer.h"
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
		virtual void OnDetach()override;
		virtual void OnEvent(Event& event) override;
		//virtual void OnUpdate() override;
		void Begin();
		void End();
		virtual void OnImGuiRender() override;
	private:
		float m_Time=0.0f;

	};
}
