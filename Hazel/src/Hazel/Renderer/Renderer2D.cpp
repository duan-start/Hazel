#include "hzpch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "glm/gtc/matrix_transform.hpp"
namespace Hazel {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		// TODO: texid
	};

	struct Renderer2DStorge {
		//cap
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		//asset
		Ref<Shader> TextureShader;
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Texture2D> WhitePixel;

		//index
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
	};

	static Renderer2DStorge s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();
		
		//����vao
		s_Data.QuadVertexArray = (Hazel::VertexArray::Create());

		//����vbo״̬��Ԥ���Ŀռ�ܳ��㣬���ǵ�һ�鲢û���ϴ�����
		s_Data.QuadVertexBuffer = Hazel::VertexBuffer::Create(s_Data.MaxVertices*sizeof(QuadVertex));

		//���ö�������
		{
			Hazel::BufferLayout layout = {
				//QuadVertex Set
				{ Hazel::ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float4, "a_Col" },
				{ Hazel::ShaderDataType::Float2, "a_Tex" }
			};

			s_Data.QuadVertexBuffer->SetLayout(layout);
		}

		//vao��vbo(û�����ݵ�)�������пռ�����Ե�
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		//����һ��ר�ŷ����ݵ����飨���ϣ���ʹ��ԭʼָ������Ϊ����opengl��ʱ��
		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		//indexBuffer
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		//����ı����ʾһ��Ҫ��ʱ�봫����Ƶĵ㣨�������ı��Σ�
		//���ù̶���indexֵ
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		
		//���������������ݸ��Ƶ�IndexBuffer��ע���Ǹ��ƶ��������ã�
		Ref<IndexBuffer> quadIB = IndexBuffer::Creat(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		
		//Texture OldPath
		s_Data.TextureShader= Hazel::Shader::Create("assets/shaders/Texture.glsl");
		//���ò������Ŀ���
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetUniformInt("u_texture", 0);
		//����һ������
		s_Data.WhitePixel = Texture2D::Create(1, 1);
		//������8λ
		uint32_t white = 0xffffffff;
		s_Data.WhitePixel->SetData(&white, sizeof(uint32_t));

	}


	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		//Todu:Camera(u_ViewProjection)
		// Init
		//set��upload������set�����ڻ��������趨������upload����ֱ�ӽ����ݴ��䵽gpu������
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		//s_Data.TextureShader->UnBind();
		
		//index���û�ָ�����ͳ�ʼ�������ṹ�����
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		//���ö���ָ�����Ժʹ��������ǿ��Էֿ��ģ�����ǰ��һ��Ҫ�ڳ����ǰ�棩
		//����ʵ�ʵ����ݵ�GPU���Ѿ����úõ�VBO����
		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		//ͳһbatching Rendering ��֤�������߼�
		Flush();
	}

	void Renderer2D::Flush()
	{
		//ͳһ����
		s_Data.WhitePixel->Bind();
		//�󶨰�ɫ
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}


	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{	
		DrawQuad(position, size, s_Data.WhitePixel, 1.0f, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		//ÿ��Draw���ǰѶ�Ӧ�����ݼӵ�cpu��vbo��������
		//ÿ���ı������ĸ��㣬ÿ�������Լ��ĸ�������
		s_Data.QuadVertexBufferPtr->Position = { position.x, position.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		//ָ�����һλ
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0.0f }, size,texture, 1.0f, tintColor);
	}


	//core static 
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		//TODU: Batch Rendering
		
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		
		texture->Bind();
		//�󶨰�ɫ
		(s_Data.TextureShader)->SetUniformFloat4("u_color", tintColor);
		(s_Data.TextureShader)->SetUniformFloat("u_tilingFactor", tilingFactor);

		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		s_Data.TextureShader->SetUniformMat4("u_Transform", tansform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);

	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y,1.0f }, size, rotation, s_Data.WhitePixel, 1.0f, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad(position, size, rotation, s_Data.WhitePixel, 1.0f, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x,position.y,1.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	//Rotation core
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.TextureShader->Bind();
		texture->Bind();
		//�󶨰�ɫ
		(s_Data.TextureShader)->SetUniformFloat4("u_color", tintColor);
		(s_Data.TextureShader)->SetUniformFloat("u_tilingFactor", tilingFactor);
		//�����ţ�����ת
		//���Ż�ı�������ı�����ϵ��������죬������Ҫ�����ţ���֤����ֻ�Ǳ�׼�����ϱ���
		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, { 0.f,0.f,1.0f }) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		s_Data.TextureShader->SetUniformMat4("u_Transform", tansform);
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

	
}
