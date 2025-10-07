
#include "hzpch.h"
#include <Hazel.h>
#include "Hazel/Core/EntryPoint.h"

#include "EditorLayer.h"

class HazelEditor : public Hazel::Application {
public:
	//子类构造函数执行的原理：
	// 子类在构造函数之前是一定要调用父类的构造函数的，只是是否显示调用而已
	//如果父类有default（），就可以不提父类（会自动调用）。否则就必须像下面显示调用
	//然后就是执行了一遍父类的构造函数，然后执行了一个PushLayer函数
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