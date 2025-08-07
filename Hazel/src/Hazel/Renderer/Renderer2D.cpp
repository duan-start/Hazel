#include "hzpch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "glm/gtc/matrix_transform.hpp"
namespace Hazel {

	//vetext attribute
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// TODO: texid
		glm::vec2 TexCoord;

		float TexIndex;
		//��������
		float TilingFactor;
	};

	struct Renderer2DStorge {
		//cap
		//��Ȼָ����Ƕ��ϴ����������ܴ�����ô��Ҳ��ͦ��������
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		//slots ����
		static const uint32_t MaxTextureSlots = 32;

		//asset
		Ref<Shader> TextureShader;
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Texture2D> WhitePixel;

		//index
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		//�洢ʵ�����������
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		//һ����ʼ��������
		glm::vec4 QuadVertexPositions[4];

		//����ͳ�� statictis ÿ֡������
		Renderer2D::statistics states;

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
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float4, "a_Col" },
				{ ShaderDataType::Float2, "a_Tex" },
				{ ShaderDataType::Float,  "a_TexID"},
				{ ShaderDataType::Float,  "a_Fra"}
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

		//Shader
		s_Data.TextureShader = Hazel::Shader::Create("assets/shaders/Texture.glsl");
#if OLDPATH
		//Texture OldPath
		//���ò������Ŀ���
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetUniformInt("u_texture", 0);
		//����һ������
		s_Data.WhitePixel = Texture2D::Create(1, 1);
		//������8λ
		uint32_t white = 0xffffffff;
		s_Data.WhitePixel->SetData(&white, sizeof(uint32_t));
#endif
		//Texture Batch
		//��������ȷ���ģ�ǰ�󶨣������������ǲ�ȷ���ģ���󶨣�
		int sampleID[s_Data.MaxTextureSlots];
		for (int i = 0; i < s_Data.MaxTextureSlots; i++) {
			sampleID[i] = i;
		}
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetUniformIntArray("u_texture", sampleID, s_Data.MaxTextureSlots);
		//s_Data.TextureShader->SetUniformMat4("u_Transform",glm::mat4(1.0f));

		//����һ������
		s_Data.WhitePixel = Texture2D::Create(1, 1);
		//������8λ
		uint32_t white = 0xffffffff;
		s_Data.WhitePixel->SetData(&white, sizeof(uint32_t));
		s_Data.TextureSlots[0] = s_Data.WhitePixel;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	}


	void Renderer2D::Shutdown()
	{
		//��ʼ����Ӧ�Ĳ�����shutdown��
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
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
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		
		HZ_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetViewProjection();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetUniformMat4("u_ViewProjection", viewProj);

		StartNewBactch();
		
	}

	void Renderer2D::BeginScene(const GameCamera& camera, const glm::mat4& transform)
	{
		HZ_PROFILE_FUNCTION();
		//����Σ����
		auto& viewpro = camera.GetProjection() * glm::inverse(transform);
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetUniformMat4("u_ViewProjection", viewpro);

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
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
		//�����еĿ���
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
			s_Data.TextureSlots[i]->Bind(i);
		}

		//����
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
		s_Data.states.DrawCalls++;
	}

	//hack
	void Renderer2D::StartNewBactch()
	{
		EndScene();
		//new set
		s_Data.QuadIndexCount = 0;
		s_Data.TextureSlotIndex = 1;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	}

	//NO Texture
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{	
		//��׳�����
		if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			//start new batch scene
			StartNewBactch();
		}

		//drawû������Ͳ��������
		const float texIndex = 0.f;//��ɫ����
		const float tiliFractor = 1.0f;

		//ÿ��Draw���ǰѶ�Ӧ�����ݼӵ�cpu��vbo��������
		//ÿ���ı������ĸ��㣬ÿ�������Լ��ĸ�������
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });


		//vec4->vec3��ʵ�ּ򵥵Ľضϣ����һ������һ�����ԣ���Ϊtransform ��һ�����Ա任λ�õľ���
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tiliFractor;
		//ָ�����һλ
		s_Data.QuadVertexBufferPtr++;

		
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tiliFractor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tiliFractor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tiliFractor;
		s_Data.QuadVertexBufferPtr++;

		//
		s_Data.QuadIndexCount += 6;

		s_Data.states.QuadCount++;
	}


	//subtexture
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, float tilingFactor , const glm::vec4& tintColor) {
		DrawQuad({ position.x,position.y,0.0f }, size, texture, 1.0f, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor) {
		HZ_PROFILE_FUNCTION();

		//��׳�����
		if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			//start new batch scene
			StartNewBactch();
		}
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture2D();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[0];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[1];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[2];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[3];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.states.QuadCount++;
	 }
	//Rotation subtexture
	 void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor , const glm::vec4& tintColor ) {

	}
	 void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor) {
		 //��׳�����
		 if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			 //start new batch scene
			 StartNewBactch();
		 }

		 const glm::vec2* textureCoords = subtexture->GetTexCoords();
		 const Ref<Texture2D> texture = subtexture->GetTexture2D();

		 //�����ţ�����ת
		 //���Ż�ı�������ı�����ϵ��������죬������Ҫ�����ţ���֤����ֻ�Ǳ�׼�����ϱ���
		 glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, { 0.f,0.f,1.0f }) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		 //glm::vec4 tmp = tansform * glm::vec4(position, 1.0f);
		 ////��̫�ø��ð�
		 //glm::vec3 pos = { tmp.x / tmp.w,tmp.y / tmp.w,tmp.z / tmp.w };
		 //DrawQuad(pos, size, texture, tilingFactor, tintColor);

		 //���������Ƿ��Ѿ���ӵ�
		 float textureIndex = 0.0f;
		 if (*texture.get() == *s_Data.TextureSlots[0].get())
			 textureIndex == 0.0f;
		 else {
			 for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			 {
				 if (*s_Data.TextureSlots[i].get() == *texture.get())
				 {
					 textureIndex = (float)i;
					 break;
				 }
			 }

			 //�������SLOTIndex��Ӧ
			 if (textureIndex == 0.0f)
			 {
				 textureIndex = (float)s_Data.TextureSlotIndex;
				 s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
				 s_Data.TextureSlotIndex++;
			 }
		 }
		 s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		 s_Data.QuadVertexBufferPtr->Color = tintColor;
		 s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		 s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		 s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		 s_Data.QuadVertexBufferPtr++;

		 s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		 s_Data.QuadVertexBufferPtr->Color = tintColor;
		 s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		 s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		 s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		 s_Data.QuadVertexBufferPtr++;

		 s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		 s_Data.QuadVertexBufferPtr->Color = tintColor;
		 s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		 s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		 s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		 s_Data.QuadVertexBufferPtr++;

		 s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		 s_Data.QuadVertexBufferPtr->Color = tintColor;
		 s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		 s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		 s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		 s_Data.QuadVertexBufferPtr++;

		 s_Data.QuadIndexCount += 6;
		 s_Data.states.QuadCount++;
	}




	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0.f }, size, color);
	}

	//White Texture;
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0.0f }, size,texture, 1.0f, tintColor);
	}


	
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		//TODU: Batch Rendering
		HZ_PROFILE_FUNCTION();

		//��׳�����
		if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			//start new batch scene
			StartNewBactch();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);

#if OLDPATH
		texture->Bind();
		//�󶨰�ɫ
		(s_Data.TextureShader)->SetUniformFloat4("u_color", tintColor);
		(s_Data.TextureShader)->SetUniformFloat("u_tilingFactor", tilingFactor);

		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		s_Data.TextureShader->SetUniformMat4("u_Transform", tansform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
#endif
	}

	//������η���ʽ���������е���Ϣ���Ѿ�������������
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		DrawQuad(transform, s_Data.WhitePixel,1.0f,color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{	
		//textureIndex
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		//������֪����
		constexpr glm::vec2 textureCoords[] = {
			{0,0},
			{1,0},
			{1,1},
			{0,1}
		};

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[0];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[1];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[2];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[3];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.states.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation, s_Data.WhitePixel, 1.0f, color);
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
		//��׳�����
		if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			//start new batch scene
			StartNewBactch();
		}
		//�����ţ�����ת
		//���Ż�ı�������ı�����ϵ��������죬������Ҫ�����ţ���֤����ֻ�Ǳ�׼�����ϱ���
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, { 0.f,0.f,1.0f }) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		//glm::vec4 tmp = tansform * glm::vec4(position, 1.0f);
		////��̫�ø��ð�
		//glm::vec3 pos = { tmp.x / tmp.w,tmp.y / tmp.w,tmp.z / tmp.w };
		//DrawQuad(pos, size, texture, tilingFactor, tintColor);

		//���������Ƿ��Ѿ���ӵ�
		float textureIndex = 0.0f;
		if (*texture.get() == *s_Data.TextureSlots[0].get())
			textureIndex = 0.0f;
		else {
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			//�������SLOTIndex��Ӧ
			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
				s_Data.TextureSlotIndex++;
			}
		}
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;
		s_Data.states.QuadCount++;
	}

	Renderer2D::statistics Renderer2D::GetStats()
	{
		return s_Data.states;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.states, 0, sizeof(s_Data.states));
	}

	
}
