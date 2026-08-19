#pragma once
#include "Hazel/Renderer/RendererPass.h"

namespace Hazel
{
	class OpenGLRenderPass:public RenderPass {
	public:
		OpenGLRenderPass(const RenderPassSpecification& spec) : m_Specification(spec) {};
		virtual ~OpenGLRenderPass() {};

		virtual const RenderPassSpecification& GetSpecification() const override { return m_Specification; }
	private:
		RenderPassSpecification m_Specification;
	};

}

