#pragma once
#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/GameCamera.h"
#include "Hazel/Renderer/EditorCamera.h"

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexArray.h"

#include "Hazel/Renderer/SubTexture2D.h"
#include "Hazel/Renderer/Texture.h"

#include "Hazel/Scene/Components.h"

namespace Hazel {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const OrthographicCamera& camera);
		//在渲染开始前，设置统一的摄像机
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const GameCamera& camera, const glm::mat4& transform);
		static void EndScene();
		
		//绘制
		static void Flush();

	//CIrcle
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thicness = 1.0, float fade = 0.02, int entityID = -1);

	//Line 
		static void DrawLine(const glm::vec3& p0,const glm::vec3& p1, const glm::vec4& color, int entityID = -1);

	//Line TO Rect
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

// aatribute
		static float GetLineWidth();
		static void SetLineWidth(float width);

	//基础绘制属性
	//static : color
		static void DrawQuad(const glm::vec3& position,const glm::vec2& size,const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	//static:texutre and color 
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

	//Scene+Editor
		static void DrawQuad(const glm::mat4& transform, SpriteRendererComponent& src, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int Entity = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f),int EntityId=-1);


	//subtexture
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));


	//旋转属性的添加
	//rotation: color
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	//rotation: texture and color
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		//core
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));


		struct statistics {
			uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;
			uint32_t DrawCalls = 0;

			uint32_t GetTotalVertexCount() { return (QuadCount+ CircleCount) * 4+ LineCount*2; }
			uint32_t GetTotalIndexCount() { return  (QuadCount + CircleCount) * 6 + LineCount * 2; }
		};

		static statistics GetStats();
		static void ResetStats();

	private:
		//hack function
		static void StartNewBactch();
	};
}

