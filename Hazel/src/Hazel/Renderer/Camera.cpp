#include "hzpch.h"
#include "Camera.h"

#include "OrthographicCamera.h"
#include "PerspectivegraphicCamera.h"

namespace Hazel {
	//由于正交和透视的相机的初始化参数不同（表示的意义不同），所以这比参数的定名还不是很清楚
	Camera* Camera::Create(float Left, float Right, float Bottom, float Top)
	{
		switch (s_CamerType)
		{
		case Hazel::CameraAPI::Ortho:
			return new OrthographicCamera(Left,  Right,  Bottom,  Top);
			//这里的名字应该换一下的，是内部改参数名字，还是多创建一个函数，都感觉有点冗余
		case Hazel::CameraAPI::Prspective:
			return new PerspectivegraphicCamera( Left,  Right,  Bottom,  Top);
		}
		HZ_CORE_ASSERT(false, "Unknown CameraType");
		return nullptr;

	}

	CameraAPI Camera::s_CamerType = CameraAPI::Ortho;
}