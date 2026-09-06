#pragma once
#include <Hazel/Scene/Scene.h>
#include "Hazel/Renderer/Framebuffer.h"

//用来渲染场景的类，主要是对Renderer3D的封装，提供更高层次的接口
namespace Hazel
{
	class Material;
	class SceneRenderer
	{
	public:
			static void Init();
			static void BeginScene(const Hazel::Scene* scene, const EditorCamera& camera);
			static void BeginScene(const Hazel::Scene* scene, const GameCamera* camera, const glm::mat4& transform);
			// Reads the source framebuffer's color attachment and writes the
			// tonemapped result into the target framebuffer (display chain).
			static void PostProcess(const Ref<Framebuffer>& source, const Ref<Framebuffer>& target, float exposure);
			static void EndScene();
	private:
		//static void RenderScene();
		static void RenderSky(const EditorCamera& camera);
	};

}

