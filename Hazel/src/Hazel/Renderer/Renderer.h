#pragma once
#include "RendererAPI.h"
#include "GameCamera.h"
#include "Camera.h"
#include "Shader.h"
#include "RenderCommandQueue.h"
#include "Hazel/Core/Window.h"


namespace Hazel {
	//后面可以完善，现在的的editor实际上没有用这个类，是散的使用command和renderer2d

	//既然renderer2D和render3D都是在一个空间内，好像没有必要分得那别开，直接在同一层里提交渲染命令就好了
	//至于Renderer就放一个通用的命令区就好了
	class Renderer {
	public:
		static void Init();
		static void OnWindowResize(uint32_t width,uint32_t height);

		static void BeginScene(const GameCamera& camera, const glm::mat4& transform);
		static void BeginScene(const Camera& camera, const std::pair<int, int>& aspect);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray,const glm::mat4& transform=glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		inline static Renderer& GetRenderer() { return *s_Instance; }
		void WaitAndRender();

		//CommonRender
		static void Clear() { s_RendererAPI->Clear(); }
		static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
		//trangles
		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) {
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}
		//Lines
		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) {
			s_RendererAPI->DrawLines(vertexArray, indexCount);
		};
		static void SetLineWidth(float width) {
			s_RendererAPI->SetLineWidth(width);
		};
	private:
		//static 的数据之只能类外定义
		//static RendererAPI::API m_RendererAPI;
		//不论绘制什么东西，都是必须传给shader的值
		struct SceneData{
			glm::mat4 ViewProjection;
			float CurrentTime;
			float SCR_Width;
			float SCR_Height;
		};
		//这里只是声明,和全局的static是一样的，只是有了一个使用的作用域
		static SceneData* m_SceneData;

		static RendererAPI* s_RendererAPI;
		static Renderer* s_Instance;
		RenderCommandQueue m_CommandQueue;
	};
}