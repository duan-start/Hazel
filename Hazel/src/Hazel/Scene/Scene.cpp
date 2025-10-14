#include "hzpch.h"
#include "Scene.h"
#include "Components.h"
#include "ScriptableEntity.h"
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

		//���´���ע�������ʵ�壬����ԭʵ���component���Ƶ��µ�ʵ������
		template<typename Component>
		static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			auto view = src.view<Component>();
			//�������e���κ�һ����component��ʵ��
			for (auto e : view)
			{
				//����uuid�õ�src��ʵ�ʵ�entityid
				//Ȼ��ֱ�Ӹ��Ƶ��½���ʵ����
				//��ζ����������ʵ���ID��һ����
				//���Բ���ֻ��ͬʱ���֣���Ⱦ�� Ҳȷʵ����
				UUID uuid = src.get<IDComponent>(e).ID;
				//HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
				entt::entity dstEnttID = enttMap.at(uuid);
				//ȥsrc�����ҵ���Ӧ��ʵ������
				auto& component = src.get<Component>(e);
				//���ҵ���������ص�Ŀ���ʵ����
				dst.emplace_or_replace<Component>(dstEnttID, component);
			}
		}

		//����ʵ������ĸ���
		//��ͬһ����������Ⱦ����������dst��Ҫ��������
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
		//ɾ�����������ָ��
		delete m_PhysicsWorld;
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		//����ָ��
		Ref<Scene> newScene = CreateRef<Scene>();

		//Camera����aspect Ration
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
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

	//ʵ��ĸ��ƣ�����ĸ��ƣ�
	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		Utils::CopyComponentIfExists<TransformComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<CameraComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		Utils::CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnRuntimeStart()
	{//physical set
		OnSimulationStart();
	}

	void Scene::OnRuntimeStop()
	{
		OnSimulationStop();
	}

	void Scene::OnSimulationStart()
	{
		//#Ifdef 3D.....
		// OnPhysics3DStart();
	
		//2D
		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnUpdateSimulation(Timestep ts,EditorCamera& camera)
	{
		// Physics update
		
		//ÿ֡�ٶȺ�λ�õ����Ĵ���
		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

		// Retrieve transform from Box2D
		//2D����
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			//��ó�ʼ��λ�ã��������Ϣ
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			//�� Rigidbody2DComponent �������ʱָ�루ͨ��ָ��void*��ת���� Box2D �ĸ���ָ��
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			//��ȡ���н�������������������λ�ã�
			const auto& position = body->GetPosition();
			transform.Translation.x = position.x;
			transform.Translation.y = position.y;
			transform.Rotation.z = body->GetAngle();
		}
		
		//Render tick
		RenderScene(camera);
	}


	//��Ϸ��ticK
	void Scene::OnUpdateRuntime(Timestep ts)
	{
		//Ϊÿһ������������ʵ�����lamda�������ű��ĸ��£�
		//stript update
		//����ÿһ�������ű���ʵ�壬���սű��߼�����tick          (ʵ�壬  �ű����)
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					//�����ű�ʵ��
					nsc.Instance=nsc.InstantiateScript();
					//���ýű�ʵ����ʵ��
					nsc.Instance->m_Entity = Entity{ entity, this };
					//nsc.Instance->OnCreate();
				}
				//���ű����߼�����ʵ��
					nsc.Instance->OnUpdate(ts);
			});

		//physical tick
		//ÿ֡�ٶȺ�λ�õ����Ĵ���
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			//2D����
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				//��ó�ʼ��λ�ã��������Ϣ
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				//�� Rigidbody2DComponent �������ʱָ�루ͨ��ָ��void*��ת���� Box2D �ĸ���ָ��
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				//��ȡ���н�������������������λ�ã�
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
				//���ص���һ����ʱ�����Ԫ��
				auto [transform, camera] = m_Registry.get<TransformComponent, CameraComponent>(entity);
				if (camera.Primary) {
					cameraTransform= transform.GetTransform();
					mainCamera = &camera.Camera;
					break;
				}
			}
			//Renderer With mainCamera
			//�����ܲ���������
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
					//ͬ��
					auto group = m_Registry.group<>(entt::get<TransformComponent, CircleRendererComponent>);
					for (auto entity : group)
					{
						auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entity);

						Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
					}
				}

				Renderer2D::EndScene();
			}
		}

	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		RenderScene(camera);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewPortSize(width, height);
		}

	}


	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	//��ȡ��Ҫ�������ʵ��
	Entity Scene::GetPrimaryCameraEntity()
	{
		//ֻ��view��
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				//�����ͬ����entityID�Ļ���ʵ������һ�������У�ע����У����������ͬһ��
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				//��ײ�еĴ�С
				//boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);
				// ���������
				float halfWidth = bc2d.Size.x * transform.Scale.x;
				float halfHeight = bc2d.Size.y * transform.Scale.y;

				// ƫ������ע�⣺Box2D ʹ�� b2Vec2��
				b2Vec2 boxCenter(bc2d.Offset.x, bc2d.Offset.y);

				// ������ƫ�Ƶ���ײ��
				boxShape.SetAsBox(halfWidth, halfHeight, boxCenter, 0.0f);

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

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius * transform.Scale.x;

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

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		//��һ��ͨ�õ�EditorCamera
		//����entity��״ֱ̬�ӻ���
		Renderer2D::BeginScene(camera);
		{
			//������ܱ����groupͬʱӵ��
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

	//δ�������ػ���ӣ�ֻ�����ڲ�û�У��õ���componentAdd���Ǹ������汾
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
}