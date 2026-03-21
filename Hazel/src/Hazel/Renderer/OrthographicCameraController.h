#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/keyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "Hazel/Core/Timestep.h"

namespace Hazel {

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

		void OnResize(uint32_t width, uint32_t height);

		OrthographicCamera& GetCamera() {
			return m_Camera;
		}
		const OrthographicCamera& GetCamera()const{
			return m_Camera;
		}
		//设置缩放比例，如果m_ZoomLevel=1，默认高为1
		//当m_ZoomLevel放大时，画面整体缩小，渲染出更多的物体
		const float GetZoomLevel()const { return m_ZoomLevel; }
		void SetZoomLevel(float zoom) { m_ZoomLevel = zoom; }

		const OrthographicCameraBounds& GetBounds() const{ return m_Bounds; }

	private:
		//回调的事件处理函数
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:
		//和窗口比例一致，否则就会变形
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		//这个顺序不能乱，相机的初始化需要前面的参数
		//初始化严格按照声明的顺序
		
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;
		//考虑使用者如果要用的话

		bool m_rotation=false;
		float m_Rotation{0.f};
		//和窗口事件的交互
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 45.0f;
		glm::vec3 m_CameraPos{0.f,0.f,0.f};


	};
}

