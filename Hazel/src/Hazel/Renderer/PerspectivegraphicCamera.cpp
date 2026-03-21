#include "hzpch.h"
#include "PerspectivegraphicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {
	PerspectivegraphicCamera::PerspectivegraphicCamera(float fov, float aspect, float zNear, float zFar):m_ProjectionMatrix(glm::perspective(glm::radians(fov), aspect, zNear, zFar)),m_ViewMatrix(1.0f)
	{
		RecalculateViewMatrix();
	}
	void PerspectivegraphicCamera::SetProjection(const glm::vec4& screen)
	{
		m_ProjectionMatrix = (glm::perspective(screen.x, screen.y, screen.z, screen.w));
		RecalculateViewMatrix();
	}
	void PerspectivegraphicCamera::SetRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;
		RecalculateViewMatrix();
	}
	void PerspectivegraphicCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		RecalculateViewMatrix();
	}
	void PerspectivegraphicCamera::RecalculateViewMatrix() 
	{
		//正面计算,由于这边是设置摄像机的位置，后面要做逆，所以是先旋转后平移，从右自左
		glm::mat4 transform = glm::translate(glm::mat4(1.0), m_Position) * glm::rotate(glm::mat4(1.0), glm::radians(m_Rotation.z), glm::vec3(0.f,0.f,1.f));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
	
