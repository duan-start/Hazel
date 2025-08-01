#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Core/Log.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel {
	//场景层次图，主要是需要对应的场景，还有选中的实体（不同的绘制方式）
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);
		void OnImGuiRender();
		void SetContext(const Ref<Scene>& contex);
		void DrawComponents(Entity entity);
		Entity GetSelectedEntity()const {return m_SelectionContext	;}
	private:
		
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context =nullptr;
		Entity m_SelectionContext = {entt::null,nullptr};
	};
}

