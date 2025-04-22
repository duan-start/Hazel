#include "hzpch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace Hazel {
	Shader* Shader::Create(const std::string& vertexsrc, const std::string& fragmentsrc) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLShader(vertexsrc, fragmentsrc);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	};

	Shader::~Shader()
	{
	}

}