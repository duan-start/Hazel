#pragma once

#include "Hazel/Renderer/Camera.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/ApplicationEvent.h"

namespace Hazel {

	class CameraController {
	public:
		CameraController() = default;
		CameraController(CameraAPI type, float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		Ref<Camera> GetCamera() { return m_Camera; }
		const Ref<Camera> GetCamera() const { return m_Camera; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		void UpdateCameraProjection();

	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		bool m_RotationEnabled = false;

		glm::vec3 m_CameraPosition{0.0f};
		float m_CameraRotation = 0.0f;

		float m_TranslationSpeed = 5.0f;
		float m_RotationSpeed = 90.0f;

		CameraAPI m_Type;
		Ref<Camera> m_Camera;
	};

}
