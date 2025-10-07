#pragma once 

#include "Hazel/Core/Layer.h"
#include "imgui.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/keyEvent.h"
#include "Hazel/Events/ApplicationEvent.h"
namespace Hazel {
	//这个是存抄案例的
	class HAZEL_API ImGuiLayer :public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnAttach()override;
		virtual void OnDetach()override;
		virtual void OnEvent(Event& event) override;
		//virtual void OnUpdate() override;
		void SetDarkThemeColors();
		void Begin();
		void End();
		virtual void OnImGuiRender() override;
	private:
		float m_Time=0.0f;

	};
}
