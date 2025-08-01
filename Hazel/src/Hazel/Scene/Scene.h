#pragma once

#include "entt.hpp"

#include "Hazel/Core/Timestep.h"

namespace Hazel {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name= "");

		void OnUpdate(Timestep ts);
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
		//声明无需定义，甚至不需要前面声明
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

	
}

