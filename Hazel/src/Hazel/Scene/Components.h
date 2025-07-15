#pragma once
#include "Hazel/Renderer/SceneCamera.h"
#include "ScriptableEntity.h"
#include <glm/glm.hpp>

namespace Hazel {

	struct TagComponent {
		std::string Tag;
		
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {
		}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct TransformComponent
	{
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {
		}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {
		}
	};

	struct CameraComponent {
		//glm::mat4 Transform{ 1.0f };
		SceneCamera Camera;
		//是否启动主视角
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	//专门创建一个scirpentity，为了实现对应的脚本函数的更新
	struct NativeScriptComponent {
		ScriptableEntity* Instance = nullptr;
		void(*InstantiateFunction)(ScriptableEntity*&) = nullptr;
		void(*DestroyInstanceFunction)(ScriptableEntity*&) = nullptr;

		//函数指针，一般用于回调和函数的实例化的延迟
		void(*OnCreateFunction)(ScriptableEntity*) = nullptr;
		void(*OnDestroyFunction)(ScriptableEntity*) = nullptr;
		void(*OnUpdateFunction)(ScriptableEntity*, Timestep) = nullptr;
		
		//BindFunction(对应的class)
		template<typename T>
		void Bind(){
			//只有std::function才能进行捕获
			//*&修改指针变量本身的一种写法
			InstantiateFunction = [](ScriptableEntity*& instance) {instance = new T(); };
			DestroyInstanceFunction = [](ScriptableEntity*& instance) {delete (T*)(instance); instance = nullptr; };

			//延迟的实例化
			//static cast适用于编译时转型(不使用多态的关系)
			OnCreateFunction = [](ScriptableEntity* instance) {static_cast<T*>(instance)->OnCreate(); };
			OnDestroyFunction = [](ScriptableEntity* instance) {static_cast<T*>(instance)->OnDestroy(); };
			OnUpdateFunction = [](ScriptableEntity* instance,Timestep ts) {static_cast<T*>(instance)->OnUpdate(ts); };

		}

	};

}