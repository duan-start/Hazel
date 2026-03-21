
#include "hzpch.h"
#include <Hazel.h>
//唯一入口
#include "Hazel/Core/EntryPoint.h"

#include "EditorLayer.h"

class XEditor : public Hazel::Application {
public:
	//子类构造函数执行的原理：
	// 子类在构造函数之前是一定要调用父类的构造函数的，只是是否显示调用而已
	//如果父类有default（），就可以不提父类（会自动调用）。否则就必须像下面显示调用
	//然后就是执行了一遍父类的构造函数，然后执行了一个PushLayer函数
	XEditor(ApplicationCommandLineArgs args):Application("X Editor", args)
	{
		//按照顺序第一个是imguiLayer
		//第二个是EditorLayer
		PushLayer(new EditorLayer());

		//PushOverlay(new ExampleLayer());
		// GameTest
		//PushOverlay(new GameLayer());
	}
	~XEditor() {

	}
};
 
//唯一定义入口layer
Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args) {
	return new XEditor(args);
}