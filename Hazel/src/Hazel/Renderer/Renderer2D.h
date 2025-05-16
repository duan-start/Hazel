#pragma once
#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexArray.h"

#include "Hazel/Renderer/Texture.h"

namespace Hazel {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

	//ª˘¥°ªÊ÷∆ Ù–‘
		static void DrawQurad(const glm::vec3& position,const glm::vec2& size,const glm::vec4& color);
		static void DrawQurad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQurad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);
		static void DrawQurad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
	};
}

