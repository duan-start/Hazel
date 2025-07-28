#pragma once

#include "RendererAPI.h"

namespace Hazel {

	//renderer ��api���м����
	class RenderCommand {
	public:
		//new
		inline static unsigned int Clear(void* datablock) {
			float* data = (float*)datablock;

			float r = *data++;
			float g = *data++;
			float b = *data++;
			float a = *data;

			s_RendererAPI->SetClearColor({ r, g, b, a });
			s_RendererAPI->Clear();
			return sizeof(float) * 4;
		}
		;
	public:

		inline static void Init() {
			s_RendererAPI->Init();
		}
		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->SetViewport( x,  y,  width,  height);
		}

		inline static void Clear() { s_RendererAPI->Clear(); }
		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		inline static	void DrawIndexed(const Ref<VertexArray>& vertexArray) {
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}