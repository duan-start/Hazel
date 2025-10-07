#include "hzpch.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>
#include "Platform/OpenGL/OpenGLShader.h"

#include "Renderer2D.h"
namespace Hazel {
	//Renderer---RenderCommad和Renderer2D。
	//通用命令和绘制命令分开


	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	//这个Renderer基本上就只用了 这一个函数
	void Renderer::Init()
	{
		HZ_PROFILE_FUNCTION();
		//调用opengl的普遍设置，开启深度测试之类的
		RenderCommand::Init();
		//专用的2d渲染器的设置和数据初始化，vao,vbo之类的
		Renderer2D::Init();
	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0,0,width,height);
	}
	void Renderer::BeginScene(const GameCamera& camera, const glm::mat4& transform)
	{
		m_SceneData->ViewProjection = camera.GetProjection() * glm::inverse(transform);
		//m_SceneData->CurrentTime = glfwGetTime();
	}
	void Renderer::BeginScene(const Camera& camera,const std::pair<int,int>& aspect)
	{
		
		m_SceneData->ViewProjection = camera.GetViewProjectionMatrix();
		//m_SceneData->CurrentTime = glfwGetTime();
	//	m_SceneData->SCR_Height = aspect.second;
		//m_SceneData->SCR_Width = aspect.first; 
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

		//youtube
		//std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat("iTime", m_SceneData->CurrentTime);
		//std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformVec2("iResolution", glm::vec2(m_SceneData->SCR_Width, m_SceneData->SCR_Height));

		RenderCommand::DrawIndexed(vertexArray);
		shader->UnBind();
	}
	//RendererAPI::API Renderer::m_RendererAPI = RendererAPI::API::OpenGL;
}