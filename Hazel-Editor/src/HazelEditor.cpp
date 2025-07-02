
#include "hzpch.h"
#include <Hazel.h>
#include "Hazel/Core/EntryPoint.h"

#include "EditorLayer.h"

class HazelEditor : public Hazel::Application {
public:
	HazelEditor():Application("Hazel Editor")
	{
		PushLayer(new EditorLayer());

		//PushOverlay(new ExampleLayer());
		
		// GameTest
		//PushOverlay(new GameLayer());
	}
	~HazelEditor() {

	}
};
 
Hazel::Application* Hazel::CreateApplication() {
	return new HazelEditor();
}