#include "hzpch.h"
#include "RendererCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Hazel {
	RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRendererAPI;

	void RendererCommand::Init()
	{
		s_RendererAPI->Init();
	}

	void RendererCommand::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(0, 0, width, height);
	}

	void RendererCommand::Clear()
	{
		s_RendererAPI->Clear();
	}

	void RendererCommand::SetClearColor(const glm::vec4& color)
	{
		s_RendererAPI->SetClearColor(color);
	}
	void RendererCommand::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		s_RendererAPI->DrawIndexed(vertexArray, indexCount);
	}
	void RendererCommand::SetLineWidth(float width)
	{
		s_RendererAPI->SetLineWidth(width);

	}
	void RendererCommand::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		s_RendererAPI->DrawLines(vertexArray, indexCount);
	}


}