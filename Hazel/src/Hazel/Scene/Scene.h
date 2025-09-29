#pragma once

#include "entt.hpp"

#include "Hazel/Core/Timestep.h"
#include "Hazel/Renderer/EditorCamera.h"

class b2World;
namespace Hazel {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name= "");

		//physical 
		void OnRuntimeStart();
		void OnRuntimeStop();


		//游戏运行的tick
		void OnUpdateRuntime(Timestep ts);
		//编辑器进行编辑的tick
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

		//声明无需定义，甚至不需要前面声明
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

	
}

