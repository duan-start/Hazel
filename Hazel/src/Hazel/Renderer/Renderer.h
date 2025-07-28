#pragma once
#include "RenderCommandQueue.h"
#include "RendererAPI.h"
#include "Camera.h"
#include "Shader.h"

#include "Hazel/Core/Window.h"


namespace Hazel {

	class Renderer {
	public:
		//repair
		static void Clear();
		static void Clear(const glm::vec4& color);
		static void ClearMagenta();
		//void Init();
		//提交渲染命令，压入渲染线程
		static void Submit(const std::function<void()>& command)
		{
			//s_Instance->m_CommandQueue.Submit(command);
		};
		void WaitAndRender();
		void SetClearColor(const glm::vec4& color);
		inline static Renderer& Get() { return *s_Instance; }
	private:
		static Renderer* s_Instance;
		RenderCommandQueue m_CommandQueue;

	//old
	public:

		static void Init();
		static void OnWindowResize(uint32_t width,uint32_t height);

		static void BeginScene(const Ref<Camera> camera, const std::pair<int, int>& aspect);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray,const glm::mat4& transform=glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		//static 的数据之只能类外定义
		//static RendererAPI::API m_RendererAPI;
		struct SceneData{
			glm::mat4 ViewProjection;
			float CurrentTime;
			float SCR_Width;
			float SCR_Height;
		};

		static SceneData* m_SceneData;
	};

//new
#define HZ_RENDER(x) ::Hazel::Renderer::Submit([=](){RenderCommandQueue::s_RendererAPI->x;})
}