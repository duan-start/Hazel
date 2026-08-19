#include "hzpch.h"
#include "RendererAPI.h"
#include "RendererPass.h"
#include "Platform/OpenGL/OpenGLRenderPass.h"

namespace Hazel
{
	 Ref<RenderPass> Hazel::RenderPass::Create(const RenderPassSpecification& spec)
	{
		 switch (RendererAPI::GetAPI())
		 {
		 case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		 case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLRenderPass>(spec);
		 }

		 HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		 return nullptr;
	 }
	
}
