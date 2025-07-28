#include "hzpch.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>
#include "Platform/OpenGL/OpenGLShader.h"

#include "Renderer2D.h"
namespace Hazel {
	//new
	Renderer* Renderer::s_Instance = new Renderer();

	void Renderer::Clear()
	{
	}
	void Renderer::Clear(const glm::vec4& color)
	{
		HZ_RENDER_IV(r, g, b, a, {
			RenderCommandQueue::s_RendererAPI->Clear({r, g, b, a});
	
	}

	
	void Renderer::ClearMagenta()
	{
		Clear({ 1, 0, 1 ,1.0});
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		// HZ_RENDER(SetClearColor(r, g, b, a));
	}

	void Renderer::WaitAndRender()
	{
		s_Instance->m_CommandQueue.Execute();
	}


	//old
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	void Renderer::Init()
	{
		/*RenderCommand::Init();
		Renderer2D::Init();*/
	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		//RenderCommand::SetViewport(0,0,width,height);
	}
	void Renderer::BeginScene(const Ref<Camera> camera,const std::pair<int,int>& aspect)
	{
		
		m_SceneData->ViewProjection = camera->GetViewProjectionMatrix();
		m_SceneData->CurrentTime = glfwGetTime();
		m_SceneData->SCR_Height = aspect.second;
		m_SceneData->SCR_Width = aspect.first; 
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray,const glm::mat4& transform)
	{	
		shader->Bind();
		vertexArray->Bind();
		
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjection);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

	

		//RenderCommand::DrawIndexed(vertexArray);
		shader->UnBind();
	}
}