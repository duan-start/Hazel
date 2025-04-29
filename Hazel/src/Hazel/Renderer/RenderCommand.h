#pragma once

#include "RendererAPI.h"

namespace Hazel {

	//renderer 和api的中间层类
	class RenderCommand {
	public:
		inline static void Init() {
			s_RendererAPI->Init();
		}
		inline static void Clear() { s_RendererAPI->Clear(); }
		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		inline static	void DrawIndexed(const Ref<VerTexArray>& vertexArray) {
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}