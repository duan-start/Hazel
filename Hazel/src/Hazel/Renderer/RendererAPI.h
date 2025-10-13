#pragma once
#include <glm/glm.hpp>
#include "VertexArray.h"

namespace Hazel {
	//和具体渲染无关的普遍命令，把大多数的东西abstract
	class RendererAPI
	{
	public:
		enum class API {
			None =0,
			OpenGL=1
		};
	public:
		virtual void Init() = 0;
		virtual void Clear() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;

		virtual void SetLineWidth(float width)=0 ;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)=0;
		//Draw
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;
		//Lines
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) =0;

		inline static API GetAPI() { return s_API; }


	private:
		static API s_API;
	};
}


