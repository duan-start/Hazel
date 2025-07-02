#include "hzpch.h"
#include "OrthographicCameraController.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseButtonCodes.h"

namespace Hazel {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation) :m_Bounds({ -aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }), m_Camera(-aspectRatio * m_ZoomLevel, aspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_AspectRatio(aspectRatio), m_rotation(rotation)
		{
		HZ_PROFILE_FUNCTION();
		}
	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		HZ_PROFILE_FUNCTION();
		//HZ_TRACE("{0}", ts.GetSeconds());
		if (Hazel::Input::IsKeyPressed(HZ_KEY_W)) {
			m_CameraPos.y += m_CameraTranslationSpeed * ts;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_S)) {
			m_CameraPos.y -= m_CameraTranslationSpeed * ts;
		}
		if (Hazel::Input::IsKeyPressed(HZ_KEY_D)) {
			m_CameraPos.x += m_CameraTranslationSpeed * ts;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_A)) {
			m_CameraPos.x -= m_CameraTranslationSpeed * ts;
		}

		if (m_rotation) {

		if (Hazel::Input::IsKeyPressed(HZ_KEY_E)) {
			m_Rotation += m_CameraRotationSpeed * ts;
		}
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_Q)) {
			m_Rotation -= m_CameraRotationSpeed * ts;
		}
		m_Camera.SetRotation(glm::vec4(0.f, 0.f, 1.0f, m_Rotation));
		}
		m_Camera.SetPosition(m_CameraPos);
		m_CameraTranslationSpeed = m_ZoomLevel*1.5f;
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		HZ_PROFILE_FUNCTION();
		//通过改变视图的大小来确定平面物体缩放的比例（透视和正交的思想倒是大差不差）
		m_ZoomLevel -= e.GetYOffset()*0.2f;
		//保证最大的放大比例，这样放大的pos的这个移动速度，（虽然fps基本都是这样的）
		m_ZoomLevel = std::fmax(0.1f, m_ZoomLevel);
		m_Bounds = { -m_AspectRatio * m_ZoomLevel,m_AspectRatio * m_ZoomLevel,-m_ZoomLevel,m_ZoomLevel };
		m_Camera.SetProjection(glm::vec4( - m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel));
		return true;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Bounds = { -m_AspectRatio * m_ZoomLevel,m_AspectRatio * m_ZoomLevel,-m_ZoomLevel,m_ZoomLevel };
		m_Camera.SetProjection(glm::vec4(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel));
		return true;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);

		//这样hack真的可以吗，感觉是轮询而不是选择
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));

		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));

	}

}
