#include "hzpch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "glm/gtc/matrix_transform.hpp"
namespace Hazel {
	struct Renderer2DStorge {
		Ref<Shader> TextureShader;
		Ref<VertexArray> QuadVertexArray;
	};

	static Renderer2DStorge* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorge();

		s_Data->QuadVertexArray = (Hazel::VertexArray::Create());

		float vertices[4 * 5] = {
			//屏幕坐标     
			-.5f,-.5f,-.5f,	0.f,0.f,//左下
			-.5f,.5f,-.5f,	0.f,1.f,//左上
			.5f,-.5f,-.5f,	1.f,0.f,//you xia
			.5f,.5f,-.5f,	1.f,1.f//you shang
		};

		Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer = Hazel::VertexBuffer::Creat(vertices, sizeof(vertices));

		{
			Hazel::BufferLayout layout = {

				{ Hazel::ShaderDataType::Float3, "a_Pos"},
				{ Hazel::ShaderDataType::Float2, "a_Tex" }
			};

			m_VertexBuffer->SetLayout(layout);
		}

		//indexbuffer
		uint32_t Index[] = { 0, 1, 2,   // first triangle
		1, 2, 3    // second triangle
		};
		Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer = Hazel::IndexBuffer::Creat(Index, sizeof(Index) / sizeof(uint32_t));

		s_Data->QuadVertexArray->AddVertexBuffer(m_VertexBuffer);
		s_Data->QuadVertexArray->SetIndexBuffer(m_IndexBuffer);

		
		s_Data->TextureShader= Hazel::Shader::Create("assets/shaders/Texture.glsl");
		//设置采样器的卡槽
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniformInt("u_texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		//set和upload的区别：set可以在缓冲区中设定，但是upload就是直接将数据传输到gpu上面了

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_Data->TextureShader->UnBind();
		
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQurad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->TextureShader->Bind();
		s_Data->QuadVertexArray->Bind();
		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		(s_Data->TextureShader)->SetUniformFloat4("u_color", color);
		s_Data->TextureShader->SetUniformMat4("u_Transform", tansform);
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQurad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQurad({ position.x,position.y ,0.0f }, size, color);
	}

	void Renderer2D::DrawQurad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		s_Data->TextureShader->Bind();
		s_Data->QuadVertexArray->Bind();
		texture->Bind();
		//绑定白色
		(s_Data->TextureShader)->SetUniformFloat4("u_color",glm::vec4(1.f));
		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		s_Data->TextureShader->SetUniformMat4("u_Transform", tansform);


		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQurad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQurad({ position.x,position.y,0.f }, size, texture);
	}

}
