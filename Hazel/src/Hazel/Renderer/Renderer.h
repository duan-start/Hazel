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
		//static ������ֻ֮�����ⶨ��
		//static RendererAPI::API m_RendererAPI;
	};
}