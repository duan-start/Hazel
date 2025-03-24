#pragma once
#include "RenderCommand.h"
#include "RendererAPI.h"
namespace Hazel {

	class Renderer {
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VerTexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	//private:
		//static 的数据之只能类外定义
		//static RendererAPI::API m_RendererAPI;
	};
}