#pragma once
#include "RenderCommand.h"
#include "RendererAPI.h"
#include "Camera.h"
#include "Shader.h"

#include "Hazel/Window.h"


namespace Hazel {

	class Renderer {
	public:
		static void Init();
		static void BeginScene(const Ref<Camera>& camera, const std::pair<int, int>& aspect);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VerTexArray>& vertexArray,const glm::mat4& transform=glm::mat4(1.0f));

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
}