#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/keyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "Hazel/Core/Timestep.h"

namespace Hazel {

	//���ֻ��ǿ��һ���ӿڵĹ�ϵ�Ļ�����ͨ��delegation��ʵ�ֵ�
	//Ҳ����ָ��
	//�������ǿ������һ���µ�ϵͳ�Ļ������Ƽ�ֱ��ʹ�ø��ϵĹ�ϵ��ʵ�ֵ�

	struct OrthographicCameraBounds {
		float Left, Right;
		float Bottom, Top;
		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		//ǿ�ҵ�Ĭ�����ԣ��󲿷�����²���Ҫ�������
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
		//�ص����¼�������
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:
		//��������Ҫʵʱ�����ģ����Ӹ���Ա�б��ʼ����
		float m_AspectRatio;
		//��������ڹ̶��Ĳ����Ļ�������ʹ�ã���ͨ�����Ը���Ĭ��ֵ�����������
		float m_ZoomLevel = 1.0f;
		//���˳�����ң�����ĳ�ʼ����Ҫǰ��Ĳ���

		//������ֱ�Ӳ��յ�Hack��������ֱ������д�����Ǹ�����ģ�ͨ��ָ��ķ�ʽ����������չ3d��Ⱦ����ʱ������ع�
		//�����г�ʼ����˳��Ҫһ��
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;
		//����ʹ�������Ҫ�õĻ�
		bool m_rotation;
		float m_Rotation{0.f};

		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 45.0f;
		glm::vec3 m_CameraPos{0.f,0.f,0.f};


	};
}

