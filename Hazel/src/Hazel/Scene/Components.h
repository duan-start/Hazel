#pragma once
#include "Hazel/Renderer/SceneCamera.h"
#include "ScriptableEntity.h"
#include "Hazel/Renderer/Texture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {
	struct IDComponent {
		uint32_t EntityID=entt::null;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(uint32_t id)
			: EntityID(id) {
		}

		operator uint32_t () { return EntityID; }
		//operator uint32_t& ()  { return EntityID; }
	};

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
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };


		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 translation)
			: Translation(translation) {
		}

		glm::mat4 GetTransform() const
		{
			/*glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });*/
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 0.0f, 1.0f, 1.0f };

		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

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

		//需要延迟定义的函数
		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);
		
		//BindFunction(对应的class)
		template<typename T>
		void Bind(){
			//由于基类不能实例化，所以我直接使用的是dynamic_cast （性能开销会大一点，运行时的原因），而static_cast 是编译时的语言
			InstantiateScript = []() { return dynamic_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}

	};

}