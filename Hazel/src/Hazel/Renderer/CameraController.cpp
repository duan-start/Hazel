#include "hzpch.h"
#include "CameraController.h"
#include "PerspectivegraphicCamera.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseButtonCodes.h"

#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/ApplicationEvent.h"


namespace Hazel {

	CameraController::CameraController(CameraAPI type, float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Type(type), m_RotationEnabled(rotation)
	{
		
		if (type == CameraAPI::Ortho)
			m_Camera = Camera::Create({ type, { -aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel} });
		else
			m_Camera = Camera::Create({ type,{45.0f, aspectRatio, 0.1f, 100.0f } });
	}

	void CameraController::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(HZ_KEY_W)) m_CameraPosition.y += m_TranslationSpeed * ts;
		if (Input::IsKeyPressed(HZ_KEY_S)) m_CameraPosition.y -= m_TranslationSpeed * ts;
		if (Input::IsKeyPressed(HZ_KEY_A)) m_CameraPosition.x -= m_TranslationSpeed * ts;
		if (Input::IsKeyPressed(HZ_KEY_D)) m_CameraPosition.x += m_TranslationSpeed * ts;

		if (m_RotationEnabled) {
			if (Input::IsKeyPressed(HZ_KEY_Q)) m_CameraRotation -= m_RotationSpeed * ts;
			if (Input::IsKeyPressed(HZ_KEY_E)) m_CameraRotation += m_RotationSpeed * ts;
			m_Camera->SetRotation(glm::vec4(0.0f, 0.0f, 1.0f, m_CameraRotation));
		}

		if (m_Camera) {
			m_Camera->SetPosition(m_CameraPosition);
		}
		else {
			// Handle the case where m_Camera is null
		}

		m_TranslationSpeed = m_ZoomLevel * 2.0f;
	}

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(CameraController::OnWindowResized));
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		if (m_Type == CameraAPI::Ortho)
		{
			m_ZoomLevel -= e.GetYOffset() * 0.25f;
			m_ZoomLevel = std::max(m_ZoomLevel, 0.1f);
			UpdateCameraProjection();
		}
		else if (m_Type == CameraAPI::Perspective)
		{
			float fov = ((PerspectivegraphicCamera*)(m_Camera.get()))->GetFov();
			fov -= e.GetYOffset() * 2.0f;
			fov = glm::clamp(fov, 15.0f, 90.0f);
			((PerspectivegraphicCamera*)(m_Camera.get()))->SetFov(fov);
			UpdateCameraProjection();
		}
		return true;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		UpdateCameraProjection();
		return true;
	}

	void CameraController::UpdateCameraProjection()
	{
		if (m_Type == CameraAPI::Ortho) {
			m_Camera->SetProjection({
				-m_AspectRatio * m_ZoomLevel,
				m_AspectRatio * m_ZoomLevel,
				-m_ZoomLevel,
				m_ZoomLevel }
			);
		}
		else {
			m_Camera->SetProjection({ ((PerspectivegraphicCamera*)(m_Camera.get()))->GetFov(), m_AspectRatio, 0.1f, 100.0f });
		}
	}

}
