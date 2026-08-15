#include "hzpch.h"
#include "Renderer3D.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Platform/OpenGL/OpenGLShader.h"
#include "Hazel/Renderer/UniformBuffer.h"
#include "Mesh.h"
#include "Renderer2D.h"

namespace Hazel {

	//在cpu端可以创建的数据及其阈值，用来batch Renderering 一次性上传给所有的gpu的数据
	struct Renderer3DStorge {
		//static const(一次性最大阈值)
		static const uint32_t MaxMeshes = 4;
		//纹理绑定的卡槽的数量
		static const uint32_t MaxTextureSlots = 32;

		//Shader（后面可以和具体的textureId集成为Material）
		Ref<Shader> PBRshader;
		Ref<Shader> SkyShader;

		//skyMap
		Ref<VertexArray> SkyVertexArray;
		Ref<VertexBuffer> SkyVertexBuffer;
		Ref<IndexBuffer> SkyIndexBuffer; // 建议加上，防止引用丢失
		//Mesh
		std::vector<Ref<Mesh>> Meshes;

		//纹理（通用）
		Ref<Texture2D> WhiteTexture;
		//存储实际纹理的索引（有点小巧合吧）
		std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
		//默认初始Index=2，第0个为默认白色纹理,第一个为默认cubemap,后面就是mesh的正常纹理
		uint32_t TextureSlotIndex = 2;

		//传到shader上面的Uniformbbuffer里面的数据
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;


		struct StaticData {
			glm::mat4 StaticViewProjection;
		};
		StaticData StaticBuffer;

		//uniformBuffer的数据(还未上传)
		Ref<UniformBuffer> CameraUniformBuffer;


		Ref<UniformBuffer> StaticUniformBuffer;
	};

	//全局唯一的
	static Renderer3DStorge s_Data;

	//Renderer---RenderCommad和Renderer2D。
	//通用命令和绘制命令分开
	Renderer3D* Renderer3D::s_Instance = new Renderer3D();

	Renderer3D::SceneData* Renderer3D::s_SceneData = new Renderer3D::SceneData();
	//这个Renderer基本上就只用了 这一个函数
	void Renderer3D::Init()
	{
		HZ_PROFILE_FUNCTION();
		//调用opengl的普遍设置，开启深度测试之类的 
		RendererCommand::Init();
		//HZ_RENDER({ RendererCommand::Init();; });（in the futuer//CommandQueue）
		ShaderLibrary::Init();
	
		//Renderer3D::Init
		{
			//Camera
			s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DStorge::CameraData), 0);
			s_Data.StaticUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DStorge::CameraData), 1);
			//ShaderInit
			//Texture
		//设置默认纹理
			s_Data.WhiteTexture = Texture2D::Create(1, 1);
			uint32_t whiteTextureData = 0xffffffff;
			s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
			//固定0号槽对应的纹理
			s_Data.TextureSlots[0] = s_Data.WhiteTexture;
			//Shader
			//shaderLib;
			auto& Lib = ShaderLibrary::GetLib();
			s_Data.PBRshader = Lib->Get("Mesh");
			s_Data.SkyShader = Lib->Get("SkyBox");

			// 立方体的 8 个顶点
			std::array<float, 8 * 3> skyboxVertices = {
				-1.0f,  1.0f, -1.0f,  // 0
				-1.0f, -1.0f, -1.0f,  // 1
				 1.0f, -1.0f, -1.0f,  // 2
				 1.0f,  1.0f, -1.0f,  // 3
				-1.0f,  1.0f,  1.0f,  // 4
				-1.0f, -1.0f,  1.0f,  // 5
				 1.0f, -1.0f,  1.0f,  // 6
				 1.0f,  1.0f,  1.0f   // 7
			};

			std::array<uint32_t, 36> skyboxIndices = {
				// 右面
				1, 2, 6, 6, 5, 1,
				// 左面
				0, 4, 7, 7, 3, 0,
				// 上面
				4, 5, 6, 6, 7, 4,
				// 下面
				0, 3, 2, 2, 1, 0,
				// 背面
				0, 1, 5, 5, 4, 0,
				// 正面
				3, 7, 6, 6, 2, 3
			};

			// skybox VAO
			s_Data.SkyVertexArray = (Hazel::VertexArray::Create());
			//创建vbo(预留最大内存)，四边形的
			s_Data.SkyVertexBuffer = Hazel::VertexBuffer::Create(skyboxVertices.data(), sizeof(float) * skyboxVertices.size());
			//设置顶点属性
			{
				Hazel::BufferLayout layout = {
					//SkyVertex Set
					{ ShaderDataType::Float3, "a_Pos"},
					//{ ShaderDataType::Float3, "a_Tex" },

				};
				//vbo设置好空间和属性
				s_Data.SkyVertexBuffer->SetLayout(layout);
			}

			//设置引用（以及顶点属性）
			s_Data.SkyVertexArray->AddVertexBuffer(s_Data.SkyVertexBuffer);

			// 4. 创建 IBO (这是你要求添加的部分)
			 s_Data.SkyIndexBuffer = Hazel::IndexBuffer::Create(skyboxIndices.data(), skyboxIndices.size());

			// 5. 将 IBO 绑定到 VAO
			s_Data.SkyVertexArray->SetIndexBuffer(s_Data.SkyIndexBuffer);

		//Test:: To Remove
		//s_Data.TextureSlots[2] = Texture2D::Create("assets/Meshes/diffuse.jpg");
		}

		//专用的2d渲染器的设置和数据初始化，vao,vbo之类的
		Renderer2D::Init();
			//HZ_RENDER({ Renderer2D::Init(); })

	}
	void Renderer3D::OnWindowResize(uint32_t width, uint32_t height)
	{
		//HZ_RENDER_2(width,height,{ s_RendererAPI->SetViewport(0,0,width,height); })
		RendererCommand::OnWindowResize(width,height);
	}
	void Renderer3D::Clear()
	{
		RendererCommand::Clear();
		//HZ_RENDER({ s_RendererAPI->Clear(); })
	
	}
	void Renderer3D::SetClearColor(const glm::vec4& color)
	{
		RendererCommand::SetClearColor(color);
	}
	void Renderer3D::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{

		RendererCommand::DrawIndexed(vertexArray, indexCount);

	}


	void Renderer3D::DrawMesh(const glm::mat4& transform, const struct MeshRendererComponent& mesh, int entityID)
	{
		//for (auto& m_mesh : s_Data.Meshes) {
		//	if (filePath == m_mesh->GetFilePath()) {
		//		targetMesh = m_mesh;
		//		break;
		//	}
		//}

		// 2. 如果没找到，新创建一个并放入容器
		//if (!targetMesh) {
		//	targetMesh = Mesh::Create(filePath);
		//	s_Data.Meshes.push_back(targetMesh);
		//}
	
		// 3. 渲染当前这个 Mesh
		s_Data.PBRshader->Bind();
		//s_Data.PBRshader->
		if(mesh.Texture)
		s_Data.TextureSlots[0] = mesh.Texture;
		s_Data.TextureSlots[0]->Bind(4);

		DrawIndexed(mesh.mesh->GetVertexArray());
	}

	void Renderer3D::RenderSkyMap(const Ref<Texture> skyMap)
	{		//如果是相同资源的话
		if (s_Data.TextureSlots[1].get() != skyMap.get()) {
			s_Data.TextureSlots[1] = skyMap;
		}
		////to do::repaire
		//glDisable(GL_BLEND);          // 必须关掉，否则颜色会被 Blend Factor 乘成全 0
		//glEnable(GL_DEPTH_TEST);      // 必须开启，配合 xyww 技巧
		//小于等于才覆盖
		glDepthFunc(GL_LEQUAL);       // 确保远平面可见
		//glDepthMask(GL_FALSE);        // 天空盒不写深度
		s_Data.SkyShader->Bind();
		//
		s_Data.TextureSlots[1]->Bind(1);
		DrawIndexed(s_Data.SkyVertexArray);

		//严格小于才覆盖
		glDepthFunc(GL_LESS); // set depth function back to default

	}

	void Renderer3D::SetLineWidth(float width)
	{
		RendererCommand::SetLineWidth(width);
	}
	void Renderer3D::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		RendererCommand::DrawLines(vertexArray, indexCount);
		
	}
	//void Renderer3D::BeginScene(const GameCamera& camera, const glm::mat4& transform)
	//{
		
		//s_SceneData->ViewProjection = camera.GetProjection() * glm::inverse(transform);
		////m_SceneData->CurrentTime = glfwGetTime();
	//}
	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		//HZ_RENDER_1(camera,{Renderer2D::BeginScene(camera);});
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.StaticBuffer.StaticViewProjection = camera.GetProjection()*glm::mat4(glm::mat3(camera.GetViewMatrix()));
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DStorge::CameraData), 0);
		s_Data.StaticUniformBuffer->SetData(&s_Data.StaticBuffer, sizeof(Renderer3DStorge::StaticBuffer), 0);

	}

	void Renderer3D::EndScene()
	{
		
	
	}
	// ||暂时没用
	void Renderer3D::Submit(const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray,const glm::mat4& transform)
	{	
		shader->Bind();
		vertexArray->Bind();
		
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjection);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		RendererCommand::DrawIndexed(vertexArray,0);
		shader->UnBind();
	}
	void Renderer3D::WaitAndRender()
	{
		//有点奇怪，因为实际上也是一样的，只是读起来意义不太明确而已  this---==s_Instance
		//m_CommandQueue.Execute();
		//全局唯一的渲染队列
		s_Instance->m_CommandQueue.Execute();
	}
	//RendererAPI::API Renderer::m_RendererAPI = RendererAPI::API::OpenGL;
}