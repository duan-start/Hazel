#pragma once
#include <Hazel/Scene/Scene.h>

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
			static void EndScene();
			//todo
			//static void SubmitMesh(const Ref<Mesh>& mesh, const glm::mat4& transform, int entityID = -1);
			static void SubmitSkyMap(const Ref<Texture>& sky);
	private:
		static void RenderScene();
		static void RenderSky(Ref<Texture> SkyMap);
	};

}

