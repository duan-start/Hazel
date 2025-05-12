#include "hzpch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "glm/gtc/matrix_transform.hpp"
namespace Hazel {
	struct Renderer2DStorge {
		Ref<Shader> FlatColoeShader;
		Ref<VertexArray> QuadVertexArray;
	};

	static Renderer2DStorge* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorge();

		s_Data->QuadVertexArray = (Hazel::VertexArray::Create());

		float vertices[4 * 3] = {
			//屏幕坐标     
			-.5f,-.5f,-.5f,//左下
			-.5f,.5f,-.5f,//左上
			.5f,-.5f,-.5f,//you xia
			.5f,.5f,-.5f//you shang
		};

		Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer = Hazel::VertexBuffer::Creat(vertices, sizeof(vertices));

		{
			Hazel::BufferLayout layout = {

				{ Hazel::ShaderDataType::Float3, "a_Pos"}
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

		s_Data->FlatColoeShader = (Hazel::Shader::Create("assets/shaders/Flat.glsl"));
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		//set和upload的区别：set可以在缓冲区中设定，但是upload就是直接将数据传输到gpu上面了
		s_Data->FlatColoeShader->Bind();
		s_Data->FlatColoeShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_Data->FlatColoeShader->UnBind();
		
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQurad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->FlatColoeShader->Bind();
		s_Data->QuadVertexArray->Bind();
		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		(s_Data->FlatColoeShader)->SetUniformFloat4("Color", color);
		s_Data->FlatColoeShader->SetUniformMat4("u_Transform", tansform);
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
		s_Data->FlatColoeShader->UnBind();
	}

	void Renderer2D::DrawQurad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQurad({ position.x,position.y ,0.0f }, size, color);
	}

}
