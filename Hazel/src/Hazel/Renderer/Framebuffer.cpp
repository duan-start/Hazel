#include "hzpch.h"

#include "Framebuffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
namespace Hazel {

	Ref<Framebuffer> Hazel::Framebuffer::Create(const FramebufferSpecification& speci)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI"); return nullptr;
		case RendererAPI::API::OpenGL: return  std::make_shared<OpenGLFramebuffer>(speci);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	}

}
