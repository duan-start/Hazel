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
		 void OnAttach();
		 void OnDettach();
		 void OnUpdate() override;
		void OnEvent(Event& event) override;
		
	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressed& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleased& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int key);
		//type,,resize
	private:
		float m_Time=0.0f;

	};
}
