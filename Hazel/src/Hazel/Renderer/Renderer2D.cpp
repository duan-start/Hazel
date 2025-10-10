#include "hzpch.h"
#include "Renderer2D.h"
#include "Hazel/Renderer/UniformBuffer.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
namespace Hazel {

	struct CircleVertex {
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;
		int EntityID;
	};

	//�����ε�ÿ�����������
	struct QuadVertex
	{//��local��λ�ã���ɫ�����������꣬����������ۣ�sampler2d��,�������𣨾����߼���shader��
		//����IDֵ��������mrt��gismosѡ��
		glm::vec3 Position;
		glm::vec4 Color;
		// TODO: texid
		glm::vec2 TexCoord;

		float TexIndex;
		//��������
		float TilingFactor;
		//Editor Only
		int EntityID;
	};

	//��cpu�ˣ�umd���˿��Դ��������ݼ�����ֵ������batch Renderering һ�����ϴ������е�gpu������
	struct Renderer2DStorge {
		//cap
		//���������ε�����
		//���������������������������Ǹ��������ģ�һ��������6��index��
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		
		//����󶨵Ŀ��۵�����
		static const uint32_t MaxTextureSlots = 32;

		//cpu��ʵ�ʴ�������Դ--ָ��
		//shader ,vao(����vbo������)��vbo(ʵ�ʵ�����)���ֶ������İ�ɫ����Ĭ�ϵ����⣬Ϊ������ϵͳ��Э���ԣ�
		Ref<Shader> QuadShader;
		Ref<Shader> CircleShader;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;

		//��ʼ����һ��״̬
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		//�洢ʵ��������������е�С�ɺϰɣ�
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		//һ����ʼ��������
		glm::vec4 QuadVertexPositions[4];

		//����shader�����Uniformbbuffer���������
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;

		//uniformBuffer������(��δ�ϴ�)
		Ref<UniformBuffer> CameraUniformBuffer;

		//����ͳ�� statictis ÿ֡������
		Renderer2D::statistics states;

	};

	//ȫ��Ψһ�ģ�һ������������ģ�˼·�ͻ������㣬������߼�ֱ�����ͺ���
	static Renderer2DStorge s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();
		
		//����vao,cpu��ָ��
		s_Data.QuadVertexArray = (Hazel::VertexArray::Create());

		//����vbo��cpu��ָ�룬Ԥ���������ڴ�
		s_Data.QuadVertexBuffer = Hazel::VertexBuffer::Create(s_Data.MaxVertices*sizeof(QuadVertex));

		//���ö�������
		{
			Hazel::BufferLayout layout = {
				//QuadVertex Set
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float4, "a_Col" },
				{ ShaderDataType::Float2, "a_Tex" },
				{ ShaderDataType::Float,  "a_TexID"},
				{ ShaderDataType::Float,  "a_Fra"},
				{ ShaderDataType::Int,  "a_EntID"}
			};
			//vbo���úÿռ������
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
		//��Ϊһ���Դ�����ı��ε��ĸ����㣬index��֤��ʱ�����
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
		
		//cpu�˵������ϴ���gpu��Ӧ��ID����ȥ
		Ref<IndexBuffer> quadIB = IndexBuffer::Creat(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);

		delete[] quadIndices;

		//Circle
		s_Data.CircleVertexArray = (Hazel::VertexArray::Create());
		s_Data.CircleVertexBuffer = Hazel::VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));

		//���ö�������
		{
			Hazel::BufferLayout layout = {
				//QuadVertex Set
				{ ShaderDataType::Float3, "a_WorldPos"},
				{ ShaderDataType::Float3, "a_LocalPos" },
				{ ShaderDataType::Float4, "a_Color" },
				{ ShaderDataType::Float,  "a_Thickness"},
				{ ShaderDataType::Float,  "a_Fade"},
				{ ShaderDataType::Int,  "a_EntID"}
			};
			//vbo���úÿռ������
			s_Data.CircleVertexBuffer->SetLayout(layout);
		}

		//vao��vbo(û�����ݵ�)�������пռ�����Ե�
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);

		//����һ��ר�ŷ����ݵ����飨���ϣ���ʹ��ԭʼָ������Ϊ����opengl��ʱ��
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB);
		
		
//shader
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;
		s_Data.QuadShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/Circle.glsl");
		//����450�Ѿ�������spirv����
		//s_Data.TextureShader = Shader::Create("assets/shaders/YouTube.glsl");
		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;


		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		//��ʽ����                     ���Զ�����ʹ��sizeof sizeof(int)
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DStorge::CameraData), 0);
	}


	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
		delete[] s_Data.CircleVertexBufferBase;
		//��ʼ����Ӧ�Ĳ�����shutdown��
		//s_Data.QuadIndexCount = 0;
		//s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		//s_Data.TextureSlotIndex = 1;
	}

	//discard
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		//Todu:Camera(u_ViewProjection)
		// Init
		//set��upload������set�����ڻ��������趨������upload����ֱ�ӽ����ݴ��䵽gpu������
		//�����������ǲ�̫�Ͻ���
		s_Data.QuadShader->Bind();
		//�Ա�uploadʵ����
		s_Data.QuadShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		StartNewBactch();

		//s_Data.TextureShader->UnBind();
		//index���û�ָ�����ͳ�ʼ�������ṹ�����
		//s_Data.QuadIndexCount = 0;
		//s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		//s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorge::CameraData),0);

		StartNewBactch();
		
	}

	void Renderer2D::BeginScene(const GameCamera& camera, const glm::mat4& transform)
	{
		HZ_PROFILE_FUNCTION();
		//���transform��campera��pos
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		//�ϴ�camerea�����ݵ�gpu;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorge::CameraData),0);

		StartNewBactch();
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();
		Flush();
	}
	//���û����߼�
	void Renderer2D::Flush()
	{  

		if (s_Data.QuadIndexCount)
		{
			//ָ��ļӼ�������Ϊ������64λ��һ��ָ��Ĵ�С��8
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			//��ʽ�ϴ�����
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			// �����е�texture
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);
			//��shader
			s_Data.QuadShader->Bind();
			//������Ҫindexbuffer
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.states.DrawCalls++;
		}
		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			s_Data.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.states.DrawCalls++;
		}
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		HZ_PROFILE_FUNCTION();

		// TODO: implement for circles
		// if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		// 	NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;

		s_Data.states.QuadCount++;
	
	}

	//hack
	void Renderer2D::StartNewBactch()
	{
		//EndScene();
		//��ʼ������
		s_Data.QuadIndexCount = 0;
		s_Data.TextureSlotIndex = 1;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

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

	void Renderer2D::DrawQuad(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	//������η���ʽ���������е���Ϣ���Ѿ�������������
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color,int Entity)
	{
		DrawQuad(transform, s_Data.WhiteTexture,1.0f,color,Entity);
	}

	//�����㵱������
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor,int Entity)
	{	
		//textureIndex
		float textureIndex = 0.0f;
		//�жϸ������Ƿ���ӹ�
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{//�ж�ָ��ָ������ݣ�������ָ��ָ��ĵ�ַ���������ڲ�������һ������
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}
		//���һ���µ�����
		if (textureIndex == 0.0f)
		{
			//��ӵ��������
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		//������֪����
		//��������ı߽�
		constexpr glm::vec2 textureCoords[] = {
			{0,0},
			{1,0},
			{1,1},
			{0,1}
		};
		//ʵʱ����vbo������
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[0];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr->EntityID = Entity;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[1];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr->EntityID = Entity;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[2];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr->EntityID = Entity;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[3];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr->EntityID = Entity;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.states.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation, s_Data.WhiteTexture, 1.0f, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad(position, size, rotation, s_Data.WhiteTexture, 1.0f, color);
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
