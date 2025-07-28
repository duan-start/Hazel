#include "hzpch.h"
#include "Camera.h"

#include "OrthographicCamera.h"
#include "PerspectivegraphicCamera.h"

namespace Hazel {



	//����������͸�ӵ�����ĳ�ʼ��������ͬ����ʾ�����岻ͬ����������Ȳ����Ķ��������Ǻ����
	Ref<Camera> Camera::Create(const CameraSpec& spec)
	{
		switch (s_CamerType)
		{
		case Hazel::CameraAPI::Ortho:
			return std::make_shared<OrthographicCamera>(spec);
			//���������Ӧ�û�һ�µģ����ڲ��Ĳ������֣����Ƕഴ��һ�����������о��е�����
		case Hazel::CameraAPI::Perspective:
			//mean fov,width/height,near,far
			return std::make_shared<PerspectivegraphicCamera>(spec);
		}
		HZ_CORE_ASSERT(false, "Unknown CameraType");
		return nullptr;

	}

	CameraAPI Camera::s_CamerType = CameraAPI::Ortho;
	//CameraAPI Camera::s_CamerType = CameraAPI::Prspective;
}