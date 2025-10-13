#pragma once

#include "entt.hpp"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/EditorCamera.h"

//前向声明
class b2World;
namespace Hazel {
	class Entity;
	//场景，实体的组合（与实体交互的类，实体复制，视口重置），tick的主要场所
	class Scene
	{
	public:
		Scene();
		~Scene();

		//Copy Scene()实现深度拷贝
		static Ref<Scene> Copy(Ref<Scene> other);
		//实现实体的复制
		void DuplicateEntity(Entity entity);

		Entity CreateEntity(const std::string& name= "");
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		//physical 
		void OnRuntimeStart();
		void OnRuntimeStop();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.group<>(entt::get<Components...>);
		}

		
		//游戏运行的tick
		void OnUpdateRuntime(Timestep ts);
		//编辑器进行编辑的tick
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		//这个是ui的大小，最后渲染出纹理之后再重新分布到ui上，能够保证完全不变型
		void OnViewportResize(uint32_t width, uint32_t height);
		//删除实体内容
		void DestroyEntity(Entity entity);
		Entity GetPrimaryCameraEntity();

	private:
		template<typename T>
		//为对应的实体添加对应的组件
		void OnComponentAdded(Entity entity,T& component);
	private:
		entt::registry m_Registry;
		//
		uint32_t m_ViewportWidth=0, m_ViewportHeight=0;

	private:
		b2World* m_PhysicsWorld = nullptr;

		//声明无需定义，甚至不需要前面声明
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

	
}

