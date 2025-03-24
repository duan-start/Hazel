#include "hzpch.h"
#include "RendererAPI.h"

namespace Hazel {
	//定义的时候要指明类型
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
}