#pragma once

#include "entt.hpp"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/EditorCamera.h"

class b2World;
namespace Hazel {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		//Copy Scene()ʵ����ȿ���
		static Ref<Scene> Copy(Ref<Scene> other);
		//ʵ��ʵ��ĸ���
		void DuplicateEntity(Entity entity);

		Entity CreateEntity(const std::string& name= "");
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		//physical 
		void OnRuntimeStart();
		void OnRuntimeStop();


		//��Ϸ���е�tick
		void OnUpdateRuntime(Timestep ts);
		//�༭�����б༭��tick
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);
		void DestroyEntity(Entity entity);
		Entity GetPrimaryCameraEntity();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity,T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth=0, m_ViewportHeight=0;

	private:
		b2World* m_PhysicsWorld = nullptr;

		//�������趨�壬��������Ҫǰ������
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

	
}

