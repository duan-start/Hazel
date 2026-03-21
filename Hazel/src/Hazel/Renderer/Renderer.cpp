#include "hzpch.h"
#include "Renderer.h"
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
		//static const uint32_t MaxVertices = MaxQuads * 4;
		//static const uint32_t MaxIndices = MaxQuads * 6;
		//纹理绑定的卡槽的数量
		static const uint32_t MaxTextureSlots = 32;

		//资产引用
		//Shader
		Ref<Shader> PBRshader;

		//Mesh资源
		//Ref<VertexArray> MeshVertexArray;
		//Ref<VertexBuffer> MeshVertexBuffer;

		//纹理（通用）
		Ref<Texture2D> WhiteTexture;
		//Mesh
		std::vector<Ref<Mesh>> Meshes;
		//存储实际纹理的索引（有点小巧合吧）
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		//默认初始Index=1，第一个为默认白色纹理
		uint32_t TextureSlotIndex = 1;

		//传到shader上面的Uniformbbuffer里面的数据
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;

		//uniformBuffer的数据(还未上传)
		Ref<UniformBuffer> CameraUniformBuffer;


	};

	//全局唯一的
	static Renderer3DStorge s_Data;

	//Renderer---RenderCommad和Renderer2D。
	//通用命令和绘制命令分开
	Renderer* Renderer::s_Instance = new Renderer();

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData();
	//这个Renderer基本上就只用了 这一个函数
	void Renderer::Init()
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
			s_Data.PBRshader = Lib->Get("Pbr");
		}
	



		//专用的2d渲染器的设置和数据初始化，vao,vbo之类的
		Renderer2D::Init();
			//HZ_RENDER({ Renderer2D::Init(); })

	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		//HZ_RENDER_2(width,height,{ s_RendererAPI->SetViewport(0,0,width,height); })
		RendererCommand::OnWindowResize(width,height);
	}
	void Renderer::Clear()
	{
		RendererCommand::Clear();
		//HZ_RENDER({ s_RendererAPI->Clear(); })
	
	}
	void Renderer::SetClearColor(const glm::vec4& color)
	{
		RendererCommand::SetClearColor(color);
	}
	void Renderer::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{

		RendererCommand::DrawIndexed(vertexArray, indexCount);

	}
	void Renderer::RenderMesh(const std::string& filePath)
	{
		Ref<Mesh> targetMesh = nullptr;

		// 1. 查找是否已加载过该 Mesh
		for (auto& mesh : s_Data.Meshes) {
			if (filePath == mesh->GetFilePath()) {
				targetMesh = mesh;
				break;
			}
		}

		// 2. 如果没找到，新创建一个并放入容器
		if (!targetMesh) {
			targetMesh = CreateRef<Mesh>(filePath); // 使用 Hazel 的 CreateRef
			s_Data.Meshes.push_back(targetMesh);
		}

		// 3. 渲染当前这个 Mesh
		s_Data.PBRshader->Bind();
		DrawIndexed(targetMesh->GetVertexArray());

	}
	void Renderer::SetLineWidth(float width)
	{
		RendererCommand::SetLineWidth(width);
	}
	void Renderer::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		RendererCommand::DrawLines(vertexArray, indexCount);
		
	}
	void Renderer::BeginScene(const GameCamera& camera, const glm::mat4& transform)
	{
		//HZ_RENDER_2(camera,transform,{Renderer2D::BeginScene(camera,transform);});
		s_SceneData->ViewProjection = camera.GetProjection() * glm::inverse(transform);
		//m_SceneData->CurrentTime = glfwGetTime();
	}
	void Renderer::BeginScene(const EditorCamera& camera)
	{
		//HZ_RENDER_1(camera,{Renderer2D::BeginScene(camera);});
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DStorge::CameraData), 0);
	}

	void Renderer::EndScene()
	{
		
	
	}
	//这个还是需要的
	void Renderer::Submit(const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray,const glm::mat4& transform)
	{	
		shader->Bind();
		vertexArray->Bind();
		
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjection);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		RendererCommand::DrawIndexed(vertexArray,0);
		shader->UnBind();
	}
	void Renderer::WaitAndRender()
	{
		//有点奇怪，因为实际上也是一样的，只是读起来意义不太明确而已  this---==s_Instance
		//m_CommandQueue.Execute();
		//全局唯一的渲染队列
		s_Instance->m_CommandQueue.Execute();
	}
	//RendererAPI::API Renderer::m_RendererAPI = RendererAPI::API::OpenGL;
}