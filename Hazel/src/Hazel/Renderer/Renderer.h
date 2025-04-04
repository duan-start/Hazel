#pragma once
#include "RenderCommand.h"
#include "RendererAPI.h"
#include "Camera.h"
#include "Shader.h"

#include "Hazel/Window.h"


namespace Hazel {

	class Renderer {
	public:
		static void BeginScene(const std::shared_ptr<Camera>& camera, const std::pair<int, int>& aspect);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VerTexArray>& vertexArray);

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