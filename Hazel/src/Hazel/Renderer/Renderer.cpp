#include "hzpch.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>
#include "Platform/OpenGL/OpenGLShader.h"
namespace Hazel {
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	void Renderer::Init()
	{
		RenderCommand::Init();
	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0,0,width,height);
	}
	void Renderer::BeginScene(const Camera& camera,const std::pair<int,int>& aspect)
	{
		
		m_SceneData->ViewProjection = camera.GetViewProjectionMatrix();
		m_SceneData->CurrentTime = glfwGetTime();
		m_SceneData->SCR_Height = aspect.second;
		m_SceneData->SCR_Width = aspect.first; 
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const Ref<Shader>& shader,const Ref<VerTexArray>& vertexArray,const glm::mat4& transform)
	{	
		shader->Bind();
		vertexArray->Bind();
		
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjection);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		//youtube
		/*std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat("iTime", m_SceneData->CurrentTime);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformVec2("iResolution", glm::vec2(m_SceneData->SCR_Width, m_SceneData->SCR_Height));*/

		RenderCommand::DrawIndexed(vertexArray);
		shader->UnBind();
	}
	//RendererAPI::API Renderer::m_RendererAPI = RendererAPI::API::OpenGL;
}