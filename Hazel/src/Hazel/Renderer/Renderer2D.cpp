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

	//正方形的每个顶点的属性
	struct QuadVertex
	{//（local）位置，颜色，采样的坐标，采样的纹理槽（sampler2d）,采样级别（具体逻辑在shader）
		//还有ID值，用来给mrt给gismos选中
		glm::vec3 Position;
		glm::vec4 Color;
		// TODO: texid
		glm::vec2 TexCoord;

		float TexIndex;
		//采样级别
		float TilingFactor;
		//Editor Only
		int EntityID;
	};

	//在cpu端（umd）端可以创建的数据及其阈值，用来batch Renderering 一次性上传给所有的gpu的数据
	struct Renderer2DStorge {
		//cap
		//最大的正方形的数量
		//顶点数量，索引的数量（绘制是根据索引的，一个方形有6个index）
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		
		//纹理绑定的卡槽的数量
		static const uint32_t MaxTextureSlots = 32;

		//cpu端实际创建的资源--指针
		//shader ,vao(解释vbo的数据)，vbo(实际的数据)，手动创建的白色纹理（默认的问题，为了整体系统的协调性）
		Ref<Shader> QuadShader;
		Ref<Shader> CircleShader;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;

		//初始化的一个状态
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		//存储实际纹理的索引（有点小巧合吧）
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		//一个初始的正方形
		glm::vec4 QuadVertexPositions[4];

		//传到shader上面的Uniformbbuffer里面的数据
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;

		//uniformBuffer的数据(还未上传)
		Ref<UniformBuffer> CameraUniformBuffer;

		//数据统计 statictis 每帧的数据
		Renderer2D::statistics states;

	};

	//全局唯一的，一次性用来传输的，思路就会清晰点，按这个逻辑直接填充就好了
	static Renderer2DStorge s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();
		
		//创建vao,cpu端指针
		s_Data.QuadVertexArray = (Hazel::VertexArray::Create());

		//创建vbo，cpu端指针，预留好最大的内存
		s_Data.QuadVertexBuffer = Hazel::VertexBuffer::Create(s_Data.MaxVertices*sizeof(QuadVertex));

		//设置顶点属性
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
			//vbo设置好空间和属性
			s_Data.QuadVertexBuffer->SetLayout(layout);
		}

		//vao绑定vbo(没有数据的)，但是有空间和属性的
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		//创建一个专门放数据的数组（堆上），使用原始指针是因为传给opengl的时候
		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		//indexBuffer
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		//这里的编码表示一定要逆时针传入绘制的点（尤其是四边形）
		//设置固定的index值
		uint32_t offset = 0;
		//因为一次性传入的四边形的四个顶点，index保证逆时针绘制
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
		
		//cpu端的数据上传到gpu对应的ID里面去
		Ref<IndexBuffer> quadIB = IndexBuffer::Creat(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);

		delete[] quadIndices;

		//Circle
		s_Data.CircleVertexArray = (Hazel::VertexArray::Create());
		s_Data.CircleVertexBuffer = Hazel::VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));

		//设置顶点属性
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
			//vbo设置好空间和属性
			s_Data.CircleVertexBuffer->SetLayout(layout);
		}

		//vao绑定vbo(没有数据的)，但是有空间和属性的
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);

		//创建一个专门放数据的数组（堆上），使用原始指针是因为传给opengl的时候
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
		//不是450已经不适配spirv了捏！
		//s_Data.TextureShader = Shader::Create("assets/shaders/YouTube.glsl");
		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;


		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		//正式定义                     可以对类型使用sizeof sizeof(int)
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DStorge::CameraData), 0);
	}


	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
		delete[] s_Data.CircleVertexBufferBase;
		//初始化对应的参数（shutdown）
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
		//set和upload的区别：set可以在缓冲区中设定，但是upload就是直接将数据传输到gpu上面了
		//所以我这里是不太严谨的
		s_Data.QuadShader->Bind();
		//对标upload实际上
		s_Data.QuadShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		StartNewBactch();

		//s_Data.TextureShader->UnBind();
		//index（用户指定）和初始化其他结构体变量
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
		//这个transform是campera的pos
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		//上传camerea的数据到gpu;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorge::CameraData),0);

		StartNewBactch();
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();
		Flush();
	}
	//调用绘制逻辑
	void Renderer2D::Flush()
	{  

		if (s_Data.QuadIndexCount)
		{
			//指针的加减法，因为我们是64位，一个指针的大小是8
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			//正式上传数据
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			// 绑定所有的texture
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);
			//绑定shader
			s_Data.QuadShader->Bind();
			//绘制需要indexbuffer
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
		//初始化设置
		s_Data.QuadIndexCount = 0;
		s_Data.TextureSlotIndex = 1;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

	}

	//NO Texture
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{	
		//健壮性添加
		if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			//start new batch scene
			StartNewBactch();
		}

		//draw没有纹理和采样级别的
		const float texIndex = 0.f;//白色纹理
		const float tiliFractor = 1.0f;

		//每次Draw都是把对应的数据加到cpu的vbo数组里面
		//每个四边形由四个点，每个点有自己的各种属性
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });


		//vec4->vec3，实现简单的截断，最后一个分量一定是以（因为transform 是一个线性变换位置的矩阵）
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tiliFractor;
		//指针后移一位
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

		//健壮性添加
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
		 //健壮性添加
		 if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			 //start new batch scene
			 StartNewBactch();
		 }

		 const glm::vec2* textureCoords = subtexture->GetTexCoords();
		 const Ref<Texture2D> texture = subtexture->GetTexture2D();

		 //先缩放，后旋转
		 //缩放会改变坐标轴的比例关系，造成拉伸，所以先要做缩放，保证物体只是标准线条上变形
		 glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, { 0.f,0.f,1.0f }) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		 //glm::vec4 tmp = tansform * glm::vec4(position, 1.0f);
		 ////不太好复用啊
		 //glm::vec3 pos = { tmp.x / tmp.w,tmp.y / tmp.w,tmp.z / tmp.w };
		 //DrawQuad(pos, size, texture, tilingFactor, tintColor);

		 //查找纹理是否已经添加到
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

			 //添加纹理到SLOTIndex对应
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

		//健壮性添加
		if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			//start new batch scene
			StartNewBactch();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);

#if OLDPATH
		texture->Bind();
		//绑定白色
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

	//对于齐次方程式来讲，所有的信息都已经包含在里面了
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color,int Entity)
	{
		DrawQuad(transform, s_Data.WhiteTexture,1.0f,color,Entity);
	}

	//就拿你当典型了
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor,int Entity)
	{	
		//textureIndex
		float textureIndex = 0.0f;
		//判断该纹理是否被添加过
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{//判断指针指向的数据（而不是指针指向的地址），所以内部进行了一个重载
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}
		//这个一个新的纹理
		if (textureIndex == 0.0f)
		{
			//添加到纹理槽上
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		//编译已知常量
		//这是纹理的边界
		constexpr glm::vec2 textureCoords[] = {
			{0,0},
			{1,0},
			{1,1},
			{0,1}
		};
		//实时设置vbo的数据
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
		//健壮性添加
		if (s_Data.QuadIndexCount >= Renderer2DStorge::MaxIndices) {
			//start new batch scene
			StartNewBactch();
		}
		//先缩放，后旋转
		//缩放会改变坐标轴的比例关系，造成拉伸，所以先要做缩放，保证物体只是标准线条上变形
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, { 0.f,0.f,1.0f }) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		//glm::vec4 tmp = tansform * glm::vec4(position, 1.0f);
		////不太好复用啊
		//glm::vec3 pos = { tmp.x / tmp.w,tmp.y / tmp.w,tmp.z / tmp.w };
		//DrawQuad(pos, size, texture, tilingFactor, tintColor);

		//查找纹理是否已经添加到
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

			//添加纹理到SLOTIndex对应
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
