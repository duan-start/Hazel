#include "hzpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Hazel {
	VertexBuffer::~VertexBuffer()
	{
	}
	//创建并传输数据
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI");return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	}

	//虽然创建了，但是并没有传输数据，输入的是一个空指针
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(size);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	}

	



	IndexBuffer::~IndexBuffer()
	{
	}

	Ref<IndexBuffer> IndexBuffer::Creat(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLIndexBuffer> (indices, count);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	}

}
