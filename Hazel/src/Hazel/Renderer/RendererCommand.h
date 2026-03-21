#pragma once
#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {
	class RendererCommand
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void Clear();
		static void SetClearColor(const glm::vec4& color);
		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount);
		static void SetLineWidth(float width);
		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount);
	private:

		static RendererAPI* s_RendererAPI;
	};

}


