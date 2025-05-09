#include "hzpch.h"
#include "Camera.h"

#include "OrthographicCamera.h"
#include "PerspectivegraphicCamera.h"

namespace Hazel {
	Camera* Camera::Create(float Left, float Right, float Bottom, float Top)
	{
		switch (s_CamerType)
		{
		case Hazel::CameraAPI::Ortho:
			return new OrthographicCamera(Left,  Right,  Bottom,  Top);
			//���������Ӧ�û�һ�µģ����ڲ��Ĳ������֣����Ƕഴ��һ�����������о��е�����
		case Hazel::CameraAPI::Prspective:
			return new PerspectivegraphicCamera( Left,  Right,  Bottom,  Top);
		}
		HZ_CORE_ASSERT(false, "Unknown CameraType");
		return nullptr;

	}

	CameraAPI Camera::s_CamerType = CameraAPI::Ortho;
}