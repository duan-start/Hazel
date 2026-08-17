#pragma once

#include "entt.hpp"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Renderer/Texture.h"
//前向声明,降低编译之间的依赖
class b2World;	


namespace Hazel {


	struct Environment
	{
		static Environment Load(const std::string& filepath);
		Ref<Texture> Sky;
	};

	struct Light
	{
		glm::vec3 Direction;
		glm::vec3 Radiance;

		float Multiplier = 1.0f;
	};


	class Entity;
	//实体的容器,渲染的最大单位（==level）
	class Scene
	{
	public:
		Scene();
		~Scene();

//Copy Scene()实现深度拷贝
		static Ref<Scene> Copy(Ref<Scene> other);

//实体操作
		//实体的复制
		void DuplicateEntity(Entity entity);
		//创建实体
		Entity CreateEntity(const std::string& name= "");
		//创建拥有特定id的实体
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		//删除实体
		void DestroyEntity(Entity entity);
//查找实体
		//获取某些特征的实体
		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.group<>(entt::get<Components...>);
		}
		//获取主相机实体
		Entity GetPrimaryCameraEntity();
//Set
		//RuntimeSet
		void OnRuntimeStart();
		void OnRuntimeStop();
		//模拟Set
		void OnSimulationStart();
		void OnSimulationStop();

//Tick
		//模拟tick(物理tick)
		void OnUpdateSimulation(Timestep ts,EditorCamera& camera);
		//游戏运行的tick(摄像机由客户端进行设置)
		void OnUpdateRuntime(Timestep ts);
		//编辑器进行编辑的tick（摄像机由引擎端进行指定）
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

//Viewport
		//这个是ui的大小，最后渲染出纹理之后再重新分布到ui上，能够保证完全不变型
		void OnViewportResize(uint32_t width, uint32_t height);

		void LoadEnvironmentMap(const std::string& filePath) { m_Environment = Environment::Load(filePath); }
	private:
		//实体组件添加
		template<typename T>
		void OnComponentAdded(Entity entity,T& component);

		//2d physical tick
		void OnPhysics2DStart();
		void OnPhysics2DStop();

	private:
		//实体注册表
		entt::registry m_Registry;
		//UI
		uint32_t m_ViewportWidth=0, m_ViewportHeight=0;

		//cubeMap
		Environment m_Environment;
	private:
		//物理世界
		b2World* m_PhysicsWorld = nullptr;

		//声明无需定义，甚至不需要前面声明(没有编译之间的内存依赖)
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
		friend class SceneRenderer;
	};

	
}

