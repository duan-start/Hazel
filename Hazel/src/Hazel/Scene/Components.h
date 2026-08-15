#pragma once
#include "Hazel/Renderer/SceneCamera.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Texture.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Core/UUID.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//实体和他对应的组件
namespace Hazel {

	//导入外部的自定义渲染的组件
	struct MeshRendererComponent {
		Ref<Mesh> mesh;
		Ref<Texture2D> Texture;
		//Material

		//如果你需要什么，就明确说，不要靠自动生成
		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};

	//需要设置一种默认的
	//Material  = shader + texture + uniform
	//这边
	//struct MaterialComponent {
	//	//如果你需要什么，就明确说，不要靠自动生成
	//	MaterialComponent() = default;
	//	MaterialComponent(const  MaterialComponent&) = default;
	//
	//public:
	//	Ref<Shader> shader;
	//	// 方式 A：经典架构 - 持有共享指针（自动计费与引用）
	//	std::unordered_map<std::string, Ref<Texture2D>> textures;
	//
	//};



	struct CircleRendererComponent {
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct IDComponent {

		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		operator UUID() { return ID; }
	/*	uint32_t EntityID=entt::null;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(uint32_t id)
			: EntityID(id) {
		}

		operator uint32_t () { return EntityID; }*/
		
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
			//model矩阵内部的乘法顺序，先做scale，再做rotation,最后是translate.
			// 如果先做rotate,再做scale的话，会变形
			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct QuadRendererComponent
	{
		glm::vec4 Color{ 1.0f, 0.0f, 1.0f, 1.0f };

		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		QuadRendererComponent() = default;
		QuadRendererComponent(const QuadRendererComponent&) = default;
		QuadRendererComponent(const glm::vec4& color)
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
		//forward declartion and definition
		class ScriptableEntity* Instance = nullptr;

		//声明函数指针类型，实现晚绑定
		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);
		
		//BindFunction(对应的class)
		template<typename T>
		void Bind(){
			//由于基类不能实例化，所以我直接使用的是dynamic_cast （性能开销会大一点，运行时的原因，自己写的确实不合理，因为这一般是父类转子类用的），而static_cast 是编译时的语言（这个才是）
			//dynamic_cast 是 C++ 中 运行时类型检查和安全转换的运算符，它主要用于多态类型的指针或引用之间的转换。（有虚函数的类）
			//总结一下，dynamic_cast 一般是用来解决“运行时安全访问子类特有成员或功能”的问题，也就是 多态类型下的向下转型（Downcast）。
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}

	};

	//动力学的运动特性
	struct Rigidbody2DComponent
	{    
		//	> ⚙️ 在物理引擎（如 Box2D）中：
		//> -`Static` 质量无限大，不受力。
		//> -`Dynamic` 有质量、受重力、会碰撞。
		//> -`Kinematic` 不受力，但可以自己通过速度或代码驱动移动。							
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		//这是一个运行时指针，指向底层物理引擎创建出来的“真实刚体对象”
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	//碰撞的特性
	struct BoxCollider2DComponent
	{
		//相对于物体中心点的偏移
		glm::vec2 Offset = { 0.0f, 0.0f };
		//半高和半宽
		glm::vec2 Size = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe（okk）
		//密度
		float Density = 1.0f;
		//摩擦力
		float Friction = 0.5f;
		//能量回复
		float Restitution = 0.0f;
		//能量计算阈值
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		//和物理引擎相互关联
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};


	struct CircleCollider2DComponent
	{
		//同样以及半径
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		// TODO(Yan): move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};


}