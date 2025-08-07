#pragma once

#include "entt.hpp"

#include "Hazel/Core/Timestep.h"
#include "Hazel/Renderer/EditorCamera.h"

namespace Hazel {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name= "");

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
		//�������趨�壬��������Ҫǰ������
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

	
}

