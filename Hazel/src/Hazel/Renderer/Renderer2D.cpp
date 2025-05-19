#include "hzpch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "glm/gtc/matrix_transform.hpp"
namespace Hazel {
	struct Renderer2DStorge {
		Ref<Shader> TextureShader;
		Ref<VertexArray> QuadVertexArray;
		Ref<Texture2D> WhitePixel;
	};

	static Renderer2DStorge* s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();
		s_Data = new Renderer2DStorge();
		s_Data->QuadVertexArray = (Hazel::VertexArray::Create());

		float vertices[4 * 5] = {
			//��Ļ����     
			-.5f,-.5f,-.5f,	0.f,0.f,//����
			-.5f,.5f,-.5f,	0.f,1.f,//����
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
		//���ò������Ŀ���
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniformInt("u_texture", 0);


		//����һ������
		s_Data->WhitePixel = Texture2D::Create(1, 1);
		//������8λ
		uint32_t white = 0xffffffff;
		s_Data->WhitePixel->SetData(&white, sizeof(uint32_t));

	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();
		//set��upload������set�����ڻ��������趨������upload����ֱ�ӽ����ݴ��䵽gpu������

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_Data->TextureShader->UnBind();
		
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		s_Data->TextureShader->Bind();
		s_Data->QuadVertexArray->Bind();
		s_Data->WhitePixel->Bind();
		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		(s_Data->TextureShader)->SetUniformFloat4("u_color", color);
		s_Data->TextureShader->SetUniformMat4("u_Transform", tansform);
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y ,0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		HZ_PROFILE_FUNCTION();

		s_Data->TextureShader->Bind();
		s_Data->QuadVertexArray->Bind();
		texture->Bind();
		//�󶨰�ɫ
		(s_Data->TextureShader)->SetUniformFloat4("u_color",glm::vec4(1.f));
		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		s_Data->TextureShader->SetUniformMat4("u_Transform", tansform);


		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x,position.y,0.f }, size, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		s_Data->TextureShader->Bind();
		s_Data->QuadVertexArray->Bind();
		texture->Bind();
		//�󶨰�ɫ
		(s_Data->TextureShader)->SetUniformFloat4("u_color", color);

		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f })*glm::rotate(glm::mat4(1.0f),rotation,{0.f,0.f,1.0f});

		s_Data->TextureShader->SetUniformMat4("u_Transform", tansform);
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawQuad({position.x, position.y, 0.f
		}, size, rotation, s_Data->WhitePixel, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 0.f
			}, size, rotation, texture, glm::vec4(1.0f));
	}

}
