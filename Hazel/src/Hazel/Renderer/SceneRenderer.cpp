#include "hzpch.h"
#include "SceneRenderer.h"


#include "Hazel/Renderer/Renderer3D.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Renderer/Material.h"

#include "Hazel/Scene/Components.h"
namespace Hazel
{
	struct SceneRendererData
	{
		const Scene* ActiveScene;
		Ref<MaterialInstance> skyBox;
		const GameCamera* mainCamera;
		const EditorCamera* editorCamera;
	};
	static SceneRendererData s_Data ;

	void Init() {
		s_Data.skyBox=MaterialInstance::Create(ShaderLibrary::GetLib()->Get("skyBox"));
	};
	void SceneRenderer::BeginScene(const Hazel::Scene* scene, const EditorCamera& camera)
	{
		//Init Scene;
		s_Data.ActiveScene = scene;
		s_Data.editorCamera = & camera;
	//	auto& m_Scene = const_cast<Scene*>( s_Data.ActiveScene);
		//Renderer3D
		Scene* m_Scene = const_cast<Scene*>(s_Data.ActiveScene);



		Renderer3D::BeginScene(camera);

		if (s_Data.ActiveScene->m_Environment.Sky);
			//Renderer3D::DrawSkyMap(m_Scene->m_Environment.Sky);
		//else HZ_CORE_INFO("NO SkyMap");
		//Render Mesh
		auto group = m_Scene->m_Registry.group<>(entt::get<TransformComponent, MeshRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, meshComp] = group.get<TransformComponent, MeshRendererComponent>(entity);

			//Renderer3D::DrawMesh(transform.GetTransform(), mesh, (int)entity);
			if (meshComp.mesh)
				Renderer3D::DrawMesh(transform.GetTransform(), meshComp, (int)entity);
		}

		Renderer3D::EndScene();
		//有一个通用的EditorCamera
		//根据entity的状态直接绘制


		//Render 2D,滤镜,自定义shader之类的
		Renderer2D::BeginScene(camera);
		{
			//组件不能被多个group同时拥有
			//auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			//DrawQuad
			auto group = m_Scene->m_Registry.group<>(entt::get<TransformComponent, QuadRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, QuadRendererComponent>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite, (int)entity);
			}


		}

		// Draw circles
		{
			auto group = m_Scene->m_Registry.group<>(entt::get<TransformComponent, CircleRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	void SceneRenderer::BeginScene(const Hazel::Scene* scene, const GameCamera* mainCamera, const glm::mat4& transform)
	{
		//Init Scene;

		s_Data.ActiveScene = scene;
		Scene* m_Scene = const_cast<Scene*>(s_Data.ActiveScene);
		s_Data.mainCamera = mainCamera;


		Renderer2D::BeginScene(mainCamera->GetProjection(), transform);

		{
			auto group = m_Scene->m_Registry.group<>(entt::get<TransformComponent, QuadRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, QuadRendererComponent>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite);
			}
		}

		// Draw circles
		{
			//同理
			auto group = m_Scene->m_Registry.group<>(entt::get<TransformComponent, CircleRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	 void SceneRenderer::EndScene() {
		s_Data.ActiveScene = nullptr;
		s_Data.mainCamera = nullptr;
	}

	 void SceneRenderer::RenderSky(Ref<Texture> SkyMap)
	 {
		 s_Data.skyBox->AddTexture(SkyMap);
		 //s_Data.skyBox->SetMat3(glm::inverse(s_Data.Camera));
	 }



}