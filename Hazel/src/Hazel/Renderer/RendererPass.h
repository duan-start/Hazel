#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Renderer/Framebuffer.h"
#include <string>

namespace Hazel {

	struct RenderPassSpecification
	{
		std::string Name;
		Ref<Framebuffer> Framebuffer;
	};

	//因为vlukan和opengl是不一样的
	class RenderPass
	{
	public:
		virtual ~RenderPass() {}

		virtual const RenderPassSpecification& GetSpecification() const = 0;

		static Ref<RenderPass> Create(const RenderPassSpecification& spec);

	};

}


