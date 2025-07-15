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
		//�Ƿ��������ӽ�
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	//ר�Ŵ���һ��scirpentity��Ϊ��ʵ�ֶ�Ӧ�Ľű������ĸ���
	struct NativeScriptComponent {
		ScriptableEntity* Instance = nullptr;
		void(*InstantiateFunction)(ScriptableEntity*&) = nullptr;
		void(*DestroyInstanceFunction)(ScriptableEntity*&) = nullptr;

		//����ָ�룬һ�����ڻص��ͺ�����ʵ�������ӳ�
		void(*OnCreateFunction)(ScriptableEntity*) = nullptr;
		void(*OnDestroyFunction)(ScriptableEntity*) = nullptr;
		void(*OnUpdateFunction)(ScriptableEntity*, Timestep) = nullptr;
		
		//BindFunction(��Ӧ��class)
		template<typename T>
		void Bind(){
			//ֻ��std::function���ܽ��в���
			//*&�޸�ָ����������һ��д��
			InstantiateFunction = [](ScriptableEntity*& instance) {instance = new T(); };
			DestroyInstanceFunction = [](ScriptableEntity*& instance) {delete (T*)(instance); instance = nullptr; };

			//�ӳٵ�ʵ����
			//static cast�����ڱ���ʱת��(��ʹ�ö�̬�Ĺ�ϵ)
			OnCreateFunction = [](ScriptableEntity* instance) {static_cast<T*>(instance)->OnCreate(); };
			OnDestroyFunction = [](ScriptableEntity* instance) {static_cast<T*>(instance)->OnDestroy(); };
			OnUpdateFunction = [](ScriptableEntity* instance,Timestep ts) {static_cast<T*>(instance)->OnUpdate(ts); };

		}

	};

}