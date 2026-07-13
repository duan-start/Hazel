#pragma once
#include "Hazel/Core/UUID.h"
#include "Scene.h"
#include "Components.h"
#include "entt.hpp"

namespace Hazel {
	//Entity ,只是为了客户端的方便
	//实际可以直接去Scene里面根据ID直接去查找
	//但是为给客户端提供统一的接口
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		
		Entity(const Entity& other) = default;

		//为特定的实体添加组件
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			//组件添加
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			//回调通知，而不是每次都是帧循环检查
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;

		}

		//替换组件（）
		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			//组件替换
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			//回调通知
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		//获取特定组件
		template<typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		//判断特定组件是否存在
		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		//移除组件
		template<typename T>
		void RemoveComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		//隐式转换
		operator entt::entity() const { return m_EntityHandle; }
		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		//特定的ID，随机数，以及创建的所谓的名称(全局唯一，即使是同一Entity)
		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		//实体的handle和属于的Scene
		entt::entity m_EntityHandle= entt::null ;
		Scene* m_Scene = nullptr;
	};

}