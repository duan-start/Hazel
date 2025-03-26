#include "hzpch.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>

namespace Hazel {
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	void Renderer::BeginScene(const std::shared_ptr<Camera>& camera, const std::unique_ptr<Window>& m_Window)
	{
		
		m_SceneData->ViewProjection = camera->GetViewProjectionMatrix();
		m_SceneData->CurrentTime = glfwGetTime();
		m_SceneData->SCR_Height = m_Window->GetHeight();
		m_SceneData->SCR_Width = m_Window->GetWidth(); 
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader,const std::shared_ptr<VerTexArray>& vertexArray )
	{	
		shader->Bind();
		vertexArray->Bind();
		
		shader->setMatrix4fv("u_ViewProjection", m_SceneData->ViewProjection);
		shader->setFloat("iTime", m_SceneData->CurrentTime);
		shader->setVec2("iResolution", glm::vec2(m_SceneData->SCR_Width, m_SceneData->SCR_Height));
		RenderCommand::DrawIndexed(vertexArray);
		shader->Unbind();
	}
	//RendererAPI::API Renderer::m_RendererAPI = RendererAPI::API::OpenGL;
}