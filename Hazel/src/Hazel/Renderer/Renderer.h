#pragma once
#include "RenderCommand.h"
#include "RendererAPI.h"
#include "GameCamera.h"
#include "Camera.h"
#include "Shader.h"

#include "Hazel/Core/Window.h"


namespace Hazel {
	//后面可以完善，现在的的editor实际上没有用这个类，是散的使用command和renderer2d
	class Renderer {
	public:
		static void Init();
		static void OnWindowResize(uint32_t width,uint32_t height);

		static void BeginScene(const GameCamera& camera, const glm::mat4& transform);
		static void BeginScene(const Camera& camera, const std::pair<int, int>& aspect);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray,const glm::mat4& transform=glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		//static 的数据之只能类外定义
		//static RendererAPI::API m_RendererAPI;
		//不论绘制什么东西，都是必须传给shader的值
		//？？？
		struct SceneData{
			glm::mat4 ViewProjection;
			float CurrentTime;
			float SCR_Width;
			float SCR_Height;
		};
		//这里只是声明
		static SceneData* m_SceneData;
	};
}