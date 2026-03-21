//把外部依赖的先放在里面进行编译

#include "hzpch.h"
//imgui的glfw+opengl3的实现
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "backends/imgui_impl_opengl3.cpp"
//这里面的是一些全局的关于glfw的回调函数的设置
//所以我们不需要再次进行设置
#include "backends/imgui_impl_glfw.cpp"
