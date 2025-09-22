
#include "hzpch.h"
#include <Hazel.h>
#include "Hazel/Core/EntryPoint.h"

#include "EditorLayer.h"

class HazelEditor : public Hazel::Application {
public:
	HazelEditor(ApplicationCommandLineArgs args):Application("Hazel Editor", args)
	{
		PushLayer(new EditorLayer());

		//PushOverlay(new ExampleLayer());
		// GameTest
		//PushOverlay(new GameLayer());
	}
	~HazelEditor() {

	}
};
 
Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args) {
	return new HazelEditor(args);
}