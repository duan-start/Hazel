#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Core/Log.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel {
	//场景层次图，主要是需要对应的场景，还有选中的实体（不同的绘制方式）
	// 给客户端和底层数据交互的核心
	//Editor 专用

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		//保存场景引用
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		//Imgui 绘制
		void OnImGuiRender();

		//选中实体
		Entity GetSelectedEntity() const { return m_SelectiedEntity; }
		void SetSelectedEntity(Entity entity);
	private:
		//在左边绘制实体节点
		//以及选中实体的Component;
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:

		//仅有的数据段
		Ref<Scene> m_Context;
		//选中的具体的物体
		Entity m_SelectiedEntity;
	};
}

