#include "hzpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Hazel {
	VerTexBuffer::~VerTexBuffer()
	{
	}
	VerTexBuffer* VerTexBuffer::Creat(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI");return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLVerTexBuffer(vertices, size);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	}
	

	IndexBuffer::~IndexBuffer()
	{
	}

	IndexBuffer* IndexBuffer::Creat(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, count);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	}

}
