#pragma once
#include "GameCamera.h"

namespace Hazel {

	class SceneCamera:public GameCamera
	{
	public:
		SceneCamera();

		virtual ~SceneCamera() = default;
		
		//只能说是扩展的接口
		void SetViewPortSize(uint32_t width, uint32_t height);

		void SetOrthoGraphic(float size, float nearClip, float farClip);

		//所谓size其实就是height，也是放大缩小的根本原因
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

