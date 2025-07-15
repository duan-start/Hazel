#pragma once
#include "GameCamera.h"

namespace Hazel {

	class SceneCamera:public GameCamera
	{
	public:
		SceneCamera();

		virtual ~SceneCamera() = default;
		
		//ֻ��˵����չ�Ľӿ�
		void SetViewPortSize(uint32_t width, uint32_t height);

		void SetOrthoGraphic(float size, float nearClip, float farClip);

		//��νsize��ʵ����height��Ҳ�ǷŴ���С�ĸ���ԭ��
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
	private:
		void RecalculateProjection();
	private:
		float m_OrthographicSize = 10.f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_AspectRatio = 0.f;


	};
}

