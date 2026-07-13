#include "hzpch.h"
#include "Scene.h"
#include "Components.h"
#include "ScriptableEntity.h"
#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>
#include "Entity.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Hazel {


	namespace Utils {
		//Hazel  to   Box2D
		static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
			case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
			}

			HZ_CORE_ASSERT(false, "Unknown body type");
			return b2_staticBody;
		}

		static void DoMath(const glm::mat4& transform)
		{

		}

		static void OnTransformConstruct(entt::registry& registry, entt::entity entity)
		{

		}

		//重新创建注册表，创建实体，并将原实体的component复制到新的实体里面
		template<typename Component>
		static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			//Component就是任意type
			//找到拥有对应组件的实体
			//然后依次拷贝
			auto view = src.view<Component>();
			
			for (auto e : view)
			{
				//根据uuid得到src的实际的entityid
				//然后直接复制到新建的实体中
				//意味着这两个的实体的ID是一样的
				//所以不能只是同时出现（渲染） 也确实不会
				UUID uuid = src.get<IDComponent>(e).ID;
				//HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
				//找到目标注册表里面对应的实体
				entt::entity dstEnttID = enttMap.at(uuid);
				//去src里面找到对应的实体的组件
				auto& component = src.get<Component>(e);
				//将找到的组件挂载到目标的实体上
				dst.emplace_or_replace<Component>(dstEnttID, component);
			}
		}

		//单个实体组件的复制
		//在同一个场景中渲染出来，所以dst需要单独创建
		template<typename Component>
		static void CopyComponentIfExists(Entity dst, Entity src)
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}
	}


	Scene::Scene()
	{
#if ENTT_EXAMPLE_CODE
		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();


		if (m_Registry.has<TransformComponent>(entity))
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);


		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
#endif
	}

	Scene::~Scene()
	{
		//删除物理世界的指针
		delete m_PhysicsWorld;
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		//创建指针对象
		Ref<Scene> newScene = CreateRef<Scene>();

		//Camera设置aspect Ration
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		//获取注册表
		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		//设置资源表
		std::unordered_map<UUID, entt::entity> enttMap;

		// 利用UUID创建相同的对象（由于并不跨Scene通信，所以我们是允许UUID重复的（Scene唯一），而且这样最高效，能够精准找到对应的实体）
		//搜集所有对象
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// 注册表里面所有（的组件）的复制（依据组件去找实体，然后根据相同的UUID精准复制到对应的实体上）
		//场景复制有两种思想，一种是遍历entity进行多个componmet复制（好理解但低效）
		//二是遍历Component进行进行多个Entity的一个Componment（cache友好，基于UUID进行entity匹配)
		Utils::CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		Utils::CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		Utils::CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		Utils::CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		Utils::CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		Utils::CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		Utils::CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		Utils::CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		return newScene;
	}

	//实体复制（组件复制）
	void Scene::DuplicateEntity(Entity entity)
	{
		//创建一个新实体（由于是同一场景下，所以UUID要重新生成）
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		//实体之间的组件复制
		Utils::CopyComponentIfExists<TransformComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<CameraComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
	}
	// 创建新实体
	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}
	//指定UUID进行实体创建
	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		//创建实体
		Entity entity(m_Registry.create(), this);
		//添加基础组件
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	//这里只是其中一个，游戏里面不仅有物理还有音效等等（而且这是开始设置，并不是tick）
	void Scene::OnRuntimeStart()
	{
		//物理模拟
		OnSimulationStart();
	}

	void Scene::OnRuntimeStop()
	{
		//物理结束
		OnSimulationStop();
	}

	//物理Start
	void Scene::OnSimulationStart()
	{

		OnPhysics2DStart();
	}

	//物理结束
	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	//物理Tick
	void Scene::OnUpdateSimulation(Timestep ts,EditorCamera& camera)
	{
		// Physics update
		
		//每帧速度和位置迭代的次数
		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

		// Retrieve transform from Box2D
		//2D刚体
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			//获得初始的位置，物体的信息
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			//把 Rigidbody2DComponent 里的运行时指针（通用指针void*）转换成 Box2D 的刚体指针（这个已经在attach里面进行设置了）
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			//获取运行结果（物体在物理引擎的位置）
			const auto& position = body->GetPosition();
			transform.Translation.x = position.x;
			transform.Translation.y = position.y;
			transform.Rotation.z = body->GetAngle();
		}
		
		//Render tick
		RenderScene(camera);

	}


	//根据游戏指定的摄像机(游戏的runTime)进行tick(游戏画面的Tick)
	void Scene::OnUpdateRuntime(Timestep ts)
	{
		//为每一个有这个组件的实体进行lamda（即按脚本的更新）
		//stript update
		//对于每一个包含脚本的实体，按照脚本逻辑进行tick          (实体，  脚本组件)
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					//创建脚本实例
					nsc.Instance=nsc.InstantiateScript();
					//设置脚本实例的实体
					nsc.Instance->m_Entity = Entity{ entity, this };
					//nsc.Instance->OnCreate();
				}
				//按脚本的逻辑更新实体
					nsc.Instance->OnUpdate(ts);
			});

		//physical tick
		//每帧速度和位置迭代的次数
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			//2D刚体
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				//获得初始的位置，物体的信息
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				//把 Rigidbody2DComponent 里的运行时指针（通用指针void*）转换成 Box2D 的刚体指针
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				//获取运行结果（物体在物理引擎的位置）
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}

		//Renderering update
		GameCamera* mainCamera = nullptr;
		//Get primay camera
		glm::mat4 cameraTransform;
		{
			auto group = m_Registry.group<>(entt::get<TransformComponent, CameraComponent>);
			//getMainCamera
			for (auto entity : group){
				//返回的是一个临时对象的元组
				auto [transform, camera] = m_Registry.get<TransformComponent, CameraComponent>(entity);
				if (camera.Primary) {
					cameraTransform= transform.GetTransform();
					mainCamera = &camera.Camera;
					break;
				}
			}
			//Renderer With mainCamera
			//看看能不能缩起来
			//--------------------------------------------------------------
			if (mainCamera)
			{
				Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

				{
					auto group = m_Registry.group<>(entt::get<TransformComponent, SpriteRendererComponent>);
					for (auto entity : group)
					{
						auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

						Renderer2D::DrawQuad(transform.GetTransform(), sprite);
					}
				}

				// Draw circles
				{
					//同理
					auto group = m_Registry.group<>(entt::get<TransformComponent, CircleRendererComponent>);
					for (auto entity : group)
					{
						auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);

						Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
					}
				}

				Renderer::EndScene();
			}
		}

	}
	//根据编辑器Camera进行tick(编辑器画面的tick)
	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		RenderScene(camera);
	}

	//设置宽高比
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras（统一修改）
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewPortSize(width, height);
		}

	}

	//删除实体
	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	//获取主要的相机的实体
	Entity Scene::GetPrimaryCameraEntity()
	{
		//只读view，
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				//如果是同样的entityID的话，实际上在一个场景中（注册表中），代表的是同一个
				return Entity{ entity, this };
		}
		return {};
	}

	//物理世界模拟的开始设置
	void Scene::OnPhysics2DStart()
	{
		//设置新世界（重力）
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		//找到刚体组件
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			//刚体定义清单（就是creatInfo）
			b2BodyDef bodyDef;
			bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			//指向真实物体的指针（通过这个这个世界进行管理和控制）
			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			//开始指向
			rb2d.RuntimeBody = body;


			//设置碰撞盒
			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				//形状
				b2PolygonShape boxShape;
				//碰撞盒的大小
				//boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);
				// 计算半宽、半高
				float halfWidth = bc2d.Size.x * transform.Scale.x;
				float halfHeight = bc2d.Size.y * transform.Scale.y;

				// 偏移量（注意：Box2D 使用 b2Vec2）
				b2Vec2 boxCenter(bc2d.Offset.x, bc2d.Offset.y);

				// 创建带偏移的碰撞盒
				boxShape.SetAsBox(halfWidth, halfHeight, boxCenter, 0.0f);
				//材质属性
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				//形状
				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius * transform.Scale.x;

				//材质属性
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	//物理世界结束的删除
	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	//Editor进行RendererTick的实际内容
	void Scene::RenderScene(EditorCamera& camera)
	{

		//Renderer3D
		Renderer::BeginScene(camera);
		

		if (m_Environment.SkyMap)
			Renderer::RenderSkyMap(m_Environment.SkyMap);
		else HZ_CORE_INFO("NO SkyMap");
	//	Renderer::RenderMesh("assets/Meshes/backpack.obj");
		Renderer::EndScene();
		//有一个通用的EditorCamera
		//根据entity的状态直接绘制
		Renderer2D::BeginScene(camera);
		{
			//组件不能被多个group同时拥有
			//auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			
			//DrawQuad
			auto group = m_Registry.group<>(entt::get<TransformComponent, SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite, (int)entity);
			}


		}

		// Draw circles
		{
			auto group = m_Registry.group<>(entt::get<TransformComponent, CircleRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	//Todo:设置回调通知（Now These are garbage）
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0) {
		component.Camera.SetViewPortSize(m_ViewportWidth, m_ViewportHeight);
		}

	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity,CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	Environment Environment::Load(const std::string& filepath)
	{
		auto skyMap = TextureCube::Create(filepath);
		return { skyMap };
	}
}