
#include "hzpch.h"

#include <Hazel.h>
#include "Hazel/Core/EntryPoint.h"

#include "GameTest/GameLayer.h"
#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		PushOverlay(new Sandbox2D());

		//PushOverlay(new ExampleLayer());
		
		// GameTest
		//PushOverlay(new GameLayer());
	}
	~Sandbox() {

	}
};
 
Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}