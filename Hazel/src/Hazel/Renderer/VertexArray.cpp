#include "hzpch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Hazel{
	VerTexArray* VerTexArray::Create()
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	}
	VerTexArray::~VerTexArray()
	{
	}
}