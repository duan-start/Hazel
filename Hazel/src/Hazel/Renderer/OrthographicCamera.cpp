#include "hzpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {
	
	OrthographicCamera::OrthographicCamera(float Left, float Right, float Bottom, float Top, float Near, float Far) :m_ProjectionMatrix(glm::ortho(Left, Right, Bottom, Top, Near, Far)),m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}


	void OrthographicCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetRotation(const glm::vec4& rotation)
	{
		m_Rotation = rotation; 
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetProjection(const glm::vec4& screen)
	{
		m_ProjectionMatrix = glm::ortho(screen.x, screen.y, screen.z, screen.w);
		RecalculateViewMatrix();
	}

	

	void OrthographicCamera::RecalculateViewMatrix() 
	{
		//正面计算,由于这边是设置摄像机的位置，后面要做逆，所以是先旋转后平移，从右自左
		glm::mat4 transform = glm::translate(glm::mat4(1.0), m_Position) * glm::rotate(glm::mat4(1.0), glm::radians(m_Rotation.w), glm::vec3(m_Rotation.x, m_Rotation.y, m_Rotation.z));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}