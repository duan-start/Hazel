#include "hzpch.h"
#include "Renderer2D.h"

#include "RendererCommand.h"
#include "Hazel/Renderer/UniformBuffer.h"
#include "Shader.h"
#include "Hazel/Renderer/Material.h"


#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
namespace Hazel {

	//针对于2D 渲染的资产，
	//1. 所有shader 统一使用的binding=0 的cameraProjection
	//2. 针对QuadMateril shader（跨VAO共享的Texture）,同时由于只有binding和location 去支持更改，这里面用的是location
	//3. 虽然使用location写法简单，但是每个三角形每个顶点都要传递entityID，是比较浪费的
	//4. 但是因为批处理的缘故，我们不能每次绘制一个Quad的时候都去设置shader里面的变量（失去了批处理的意义）

	struct LineVertex {
		glm::vec3 Position;
		glm::vec4 Color;
		int EntityID;
	};
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
	{	//（local）位置，颜色，采样的坐标，采样的纹理槽（sampler2d）,采样级别（具体逻辑在shader）
		//还有ID值，用来给mrt给gismos选中
		//对应shader里面的顶点输入
		glm::vec3 Position;
		glm::vec4 Color;
		// TODO: texid
		glm::vec2 TexCoord;
		float TexIndex;
		//采样级别(平铺因子)
		float TilingFactor;
		//Editor Only
		int EntityID;
	};

//在cpu端可以创建的数据及其阈值，用来batch Renderering 一次性上传给所有的gpu的数据
	struct Renderer2DStorge {
		//static const(一次性最大阈值)
		static const uint32_t MaxQuads = 1000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;

//资产引用
		//Material
		Ref<Material> QuadMaterial;
		Ref<Material> CircleMaterial;
		Ref<Material> LineMaterial;
		//MaterialInstance
		Ref<MaterialInstance> QuadMaterialInstance;
		Ref<MaterialInstance> CircleMaterialInstance;
		Ref<MaterialInstance> LineMaterialInstance;

		//全屏quad的vao和vbo
		Ref<VertexArray> m_FullscreenQuadVertexArray;
		Ref<VertexBuffer> m_FullscreenQuadVertexBuffer;
		//方形资源
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		//圆形
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		//线段
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		


		//纹理（通用）
		Ref<Texture2D> WhiteTexture;
//实际数据统计（Init）
		//Quad
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		//Circle
		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		//Line
		uint32_t LineIndexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;
		float LineWidth = 2.f;

		//默认初始Index=1，第一个为默认白色纹理

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

	//全局唯一的，一次性用来传输的，（类内工具）
	static Renderer2DStorge s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION();

		// Create fullscreen quad
		float x = -1;
		float y = -1;
		float width = 2, height = 2;
		//单独定义，不同于之前的
		struct FullQuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		FullQuadVertex* data = new FullQuadVertex[4];

		data[0].Position = glm::vec3(x, y, 0.1f);
		data[0].TexCoord = glm::vec2(0, 0);

		data[1].Position = glm::vec3(x + width, y, 0.1f);
		data[1].TexCoord = glm::vec2(1, 0);

		data[2].Position = glm::vec3(x + width, y + height, 0.1f);
		data[2].TexCoord = glm::vec2(1, 1);

		data[3].Position = glm::vec3(x, y + height, 0.1f);
		data[3].TexCoord = glm::vec2(0, 1);

		s_Data.m_FullscreenQuadVertexArray = VertexArray::Create();
		s_Data.m_FullscreenQuadVertexBuffer = VertexBuffer::Create(4 * sizeof(FullQuadVertex));
		s_Data.m_FullscreenQuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_Data.m_FullscreenQuadVertexBuffer->SetData(data, 4 * sizeof(FullQuadVertex));
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		auto quadFullIB = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));

		s_Data.m_FullscreenQuadVertexArray->AddVertexBuffer(s_Data.m_FullscreenQuadVertexBuffer);
		s_Data.m_FullscreenQuadVertexArray->SetIndexBuffer(quadFullIB);




//设置四边形的批处理
		//创建vao（RendererID）
		s_Data.QuadVertexArray = (Hazel::VertexArray::Create());
		//创建vbo(预留最大内存)，四边形的
		s_Data.QuadVertexBuffer = Hazel::VertexBuffer::Create(s_Data.MaxVertices*sizeof(QuadVertex));
		//设置顶点属性
		{
			Hazel::BufferLayout layout = {
				//QuadVertex Set
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float4, "a_Col" },
				{ ShaderDataType::Float2, "a_Tex" },
				{ ShaderDataType::Float,  "a_TexID"},
				//a_Fra代表的是平铺因子
				{ ShaderDataType::Float,  "a_Fra"},
				{ ShaderDataType::Int,  "a_EntID"}
			};
			//vbo设置好空间和属性
			s_Data.QuadVertexBuffer->SetLayout(layout);
		}

		//设置引用（以及顶点属性）
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		//创建一个专门放数据的数组（堆上），使用原始指针是因为传给opengl的时候
		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		//indexBuffer
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		//设置index的绘制顺序（逆时针固定）---取决于你的初始顶点坐标
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
		
		//设置cpu端数据池
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		//绑定ibo
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

//设置圆形的批处理
		s_Data.CircleVertexArray = (Hazel::VertexArray::Create());
		s_Data.CircleVertexBuffer = Hazel::VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		//buffer布局
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
			//vbo捕获
			s_Data.CircleVertexBuffer->SetLayout(layout);
		}

		//vao绑定vbo（捕获顶点属性）
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		//设置cpu端数据池
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];
		//画圆形和画四边形是一个顺序
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB);
	
//设置线段的批处理
		s_Data.LineVertexArray = (Hazel::VertexArray::Create());
		s_Data.LineVertexBuffer = Hazel::VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));

		//buffer布局
		{
			Hazel::BufferLayout layout = {
				//QuadVertex Set
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float4, "a_Col" },
				{ ShaderDataType::Int,  "a_EntID"}
			};
			//vbo捕获
			s_Data.LineVertexBuffer->SetLayout(layout);
		}
		//vao捕获
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);

		//设置cpu端数据池
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];
		//Line 的Draw不需要indexBuffer（由drawCall解释就好）
		//s_Data.LineVertexArray->SetIndexBuffer(quadIB);


//MaterialInstance
		//shaderLib;
		auto& Lib = ShaderLibrary::GetLib();

		//Base Material Init
		s_Data.QuadMaterial=Material::Create(Lib->Get("Texture"));
		s_Data.QuadMaterial->SetDepthFunc(DepthFunc::Less);
		s_Data.QuadMaterial->SetDepthMask(DepthMask::True);
		s_Data.QuadMaterialInstance = MaterialInstance::Create(s_Data.QuadMaterial);

		s_Data.CircleMaterial = Material::Create(Lib->Get("Circle"));
		s_Data.CircleMaterial->SetDepthFunc(DepthFunc::Less);
		s_Data.CircleMaterial->SetDepthMask(DepthMask::True);
		s_Data.CircleMaterialInstance = MaterialInstance::Create(s_Data.CircleMaterial);

		s_Data.LineMaterial	 = Material::Create(Lib->Get("Line"));
		s_Data.LineMaterial->SetDepthFunc(DepthFunc::Less);
		s_Data.LineMaterial->SetDepthMask(DepthMask::True);
		s_Data.LineMaterialInstance = MaterialInstance::Create(s_Data.LineMaterial);

	//Texture
				//设置默认纹理
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		//固定0号槽对应的纹理
		//
		s_Data.QuadMaterialInstance->AddTexture(s_Data.WhiteTexture);


		//Quad初始顶点位置
		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		//CameraUniformBuffer(binding=0)
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DStorge::CameraData), 0);


	}


	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
		delete[] s_Data.CircleVertexBufferBase;
		delete[] s_Data.LineVertexBufferBase;
	}

	//discard now
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		//Todu:Camera(u_ViewProjection)  //old
		//s_Data.QuadShader->Bind();
		//对标upload实际上
	//	s_Data.QuadShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		//StartNewBactch();

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

	//调用DrawCall
	void Renderer2D::Flush()
	{  

		//Quad
		if (s_Data.QuadIndexCount)
		{
			//计算字节大小
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			//上传数据（按字节）
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
			//绑定对应Material(shader+RenderMode)
			//Texture+shader
			s_Data.QuadMaterialInstance->Bind();
			//调用DrawCall
			RendererCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.states.DrawCalls++;
		}
		//Circle
		if (s_Data.CircleIndexCount)
		{
			//数据大小（字节单位）
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			//传输数据
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			s_Data.CircleMaterialInstance->Bind();
			RendererCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.states.DrawCalls++;
		}
		//Line
		if (s_Data.LineIndexCount) {
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);
			s_Data.LineMaterialInstance->Bind();
			RendererCommand::SetLineWidth(s_Data.LineWidth);
			RendererCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineIndexCount);
			s_Data.states.DrawCalls++;
		}
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		HZ_PROFILE_FUNCTION();
		//HZ_RENDER_5();
		// TODO: implement for circles
		// if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		// 	NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			//以quad为画布
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;

		s_Data.states.CircleCount++;
	
	}

	void Renderer2D::DrawLine(const glm::vec3& p0,const glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		HZ_PROFILE_FUNCTION();
		//HZ_RENDER_4(p0, p1, color, entityID, {// TODO: implement for circles
		// if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		// 	NextBatch();
		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineIndexCount += 2;

		s_Data.states.LineCount++; 
	//});

	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{

		//HZ_RENDER_4(position, size, color, entityID, {// TODO: implement for circles
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	//});

	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		//HZ_RENDER_3(transform, color, entityID, {// TODO: implement for circles
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	//});

		
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		//HZ_RENDER_1(width, {// TODO: implement for circles
		//s_Data.LineWidth = width; }
		//);
		s_Data.LineWidth = width;
	}

	void Renderer2D::DrawFullscreenQuad(const Ref<MaterialInstance>& material)
	{
		material->Bind();

		RendererCommand::DrawIndexed(s_Data.m_FullscreenQuadVertexArray, 6);

	}


	//Reset
	void Renderer2D::StartNewBactch()
	{	//flush
		EndScene();
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineIndexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.QuadMaterialInstance->ResetTexture();
	}

	//NO Texture
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{	
		//健壮性添加
		if (s_Data.QuadIndexCount>= Renderer2DStorge::MaxIndices) 
			StartNewBactch();

		//默认纹理和采样级别
		const float texIndex = 0.f;
		const float tiliFractor = 1.0f;

		//变换矩阵 SRT
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		//记录顶点数据（一个Quad记录四个顶点）
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
			s_Data.QuadVertexBufferPtr->Color = color;
			//纹理uv，和实际顶点对应起来，已经flip过了（uv）
			s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tiliFractor;
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
		}
		//quad数量和Index(次数)更新
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

	
		float textureIndex=(float)s_Data.QuadMaterialInstance->AddTexture(texture);

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
		 if (s_Data.QuadIndexCount>= Renderer2DStorge::MaxIndices) {
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

		 float textureIndex=(float)s_Data.QuadMaterialInstance->AddTexture(texture);


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
		if (s_Data.QuadIndexCount +4> Renderer2DStorge::MaxIndices) {
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

	void Renderer2D::DrawQuad(const glm::mat4& transform, QuadRendererComponent& src, int entityID)
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

	//最完整的
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor,int Entity)
	{	
		//textureIndex
		float textureIndex = (float) s_Data.QuadMaterialInstance->AddTexture(texture);
		

		//编译已知常量
		//这是纹理的边界
		constexpr glm::vec2 textureCoords[] = {{0,0},{1,0},{1,1},{0,1}};
		//实时设置vbo的数据
		for (int i = 0; i < 4; i++) {
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = Entity;
			s_Data.QuadVertexBufferPtr++;
		}

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
		if (s_Data.QuadIndexCount>= Renderer2DStorge::MaxIndices) {
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
		//textureIndex
		float textureIndex = (float)s_Data.QuadMaterialInstance->AddTexture(texture);

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
