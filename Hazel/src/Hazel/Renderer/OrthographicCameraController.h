#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/keyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "Hazel/Core/Timestep.h"

namespace Hazel {

	//如果只是强调一个接口的关系的话，是通过delegation来实现的
	//也就是指针
	//但是如果强调的是一个新的系统的话，是推荐直接使用复合的关系来实现的

	struct OrthographicCameraBounds {
		float Left, Right;
		float Bottom, Top;
		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		//强烈的默认属性，大部分情况下不需要这个参数
		OrthographicCameraController(float aspectRatio,bool rotation=true);

		void OnEvent(Event& event);
		void OnUpdate(Timestep ts);

		OrthographicCamera& GetCamera() {
			return m_Camera;
		}
		const OrthographicCamera& GetCamera()const{
			return m_Camera;
		}
		const float GetZoomLevel()const { return m_ZoomLevel; }
		void SetZoomLevel(float zoom) { m_ZoomLevel = zoom; }

		const OrthographicCameraBounds& GetBounds() const{ return m_Bounds; }

	private:
		//回调的事件处理函数
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:
		//对于这种要实时给定的，就扔给成员列表初始化吧
		float m_AspectRatio;
		//如果是类内固定的参数的话（对象使用），通常可以给定默认值，这是最早的
		float m_ZoomLevel = 1.0f;
		//这个顺序不能乱，相机的初始化需要前面的参数

		//这里是直接参照的Hack，按理是直接用我写出的那个抽象的（通过指针的方式），后面扩展3d渲染器的时候可以重构
		//和序列初始化的顺序要一致
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;
		//考虑使用者如果要用的话
		bool m_rotation;
		float m_Rotation{0.f};

		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 45.0f;
		glm::vec3 m_CameraPos{0.f,0.f,0.f};


	};
}

