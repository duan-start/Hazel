#include "hzpch.h"
#include "Camera.h"

#include "OrthographicCamera.h"
#include "PerspectivegraphicCamera.h"

namespace Hazel {
    // 专门用于创建正交相机
    //正交投影固定near,far
    Camera* Camera::CreateOrtho(float left, float right, float bottom, float top) {
        return new OrthographicCamera(left, right, bottom, top);
    }
    // 专门用于创建透视相机
    Camera* Camera::CreatePerspective(float fov, float aspect, float znear, float zfar) {
        return new PerspectivegraphicCamera(fov, aspect, znear, zfar);
    }

	CameraAPI Camera::s_CamerType = CameraAPI::Ortho;
}