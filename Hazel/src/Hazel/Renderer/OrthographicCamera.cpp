#include "hzpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Hazel {

	OrthographicCamera::OrthographicCamera(float Left, float Right, float Bottom, float Top, float Near, float Far) :m_ProjectionMatrix(glm::ortho(Left, Right, Bottom, Top, Near, Far)), m_ViewMatrix(1.0f)
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
		m_Rotation = glm::vec4(rotation.x, rotation.y, rotation.z, rotation.w); // 硬编码Z轴
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetProjection(const glm::vec4& screen)
	{
		m_ProjectionMatrix = glm::ortho(screen.x, screen.y, screen.z, screen.w);
		RecalculateViewMatrix();
	}



	void OrthographicCamera::RecalculateViewMatrix()
	{
		// 正确的2D相机矩阵顺序：先旋转后平移（T * R）的逆 = R^-1 * T^-1
		//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.w),
		//	glm::vec3(0.0f, 0.0f, 1.0f)); // 强制Z轴
		//glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);

		//// 视图矩阵 = (T * R)^-1 = R^-1 * T^-1
		//m_ViewMatrix = glm::transpose(rotation) * glm::translate(glm::mat4(1.0f), -m_Position);

		glm::mat4 m_ViewMatrixInverse = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.w),
			glm::vec3(m_Rotation.x, m_Rotation.y, m_Rotation.z));
		m_ViewMatrix = glm::inverse(m_ViewMatrixInverse);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		// 调试输出

	}
}