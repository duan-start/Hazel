#include "hzpch.h"
#include "Renderer.h"

namespace Hazel {
	void Renderer::BeginScene()
	{
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const std::shared_ptr<VerTexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	//RendererAPI::API Renderer::m_RendererAPI = RendererAPI::API::OpenGL;
}