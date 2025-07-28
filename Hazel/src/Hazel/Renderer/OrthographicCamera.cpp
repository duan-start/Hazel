#include "hzpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Hazel {


	//这里简单介绍一下各个矩阵的作用，不然容易混：
	//这里的projectionMatrix作用在世界坐标上，返回的是一个四维的向量（w分量为1）
	//由于没有透视，所以只需要取其中x,y坐标映射到屏幕上，z用来做zbuffer;
	//这个参数我还没有太搞明白
	OrthographicCamera::OrthographicCamera(float Left, float Right, float Bottom, float Top) :m_ProjectionMatrix(glm::ortho(Left, Right, Bottom, Top,-1.0f,1.0f)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	OrthographicCamera::OrthographicCamera(const CameraSpec& spec):
		m_ProjectionMatrix(glm::ortho(spec.ortho.Top, spec.ortho.Right, spec.ortho.Bottom, spec.ortho.Top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
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
		//直接这样计算就好了，没有必要调用recal。。函数
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	//简单的思路：根据相机的位置和旋转角度（如果是2d的话就是一个一维的值）（这个是使用者给定的），去计算从原点到这样的矩阵（从右往左的顺序，先旋转后移动），然后求个逆，视图矩阵就写好了
	//其实这是2d简化版本的，如果是3d的话，相机的坐标是一个维度为3的向量，相机的方向也是一个维度为3的向量
	void OrthographicCamera::RecalculateViewMatrix()
	{
		// 正确的2D相机矩阵顺序：先旋转后平移（T * R）的逆 = R^-1 * T^-1
		//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.w),
		//	glm::vec3(0.0f, 0.0f, 1.0f)); // 强制Z轴
		//glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);

		//// 视图矩阵 = (T * R)^-1 = R^-1 * T^-1
		//m_ViewMatrix = glm::transpose(rotation) * glm::translate(glm::mat4(1.0f), -m_Position);


		//m_ViewProjection=glm::inverse(m_ViewMatrixInverse*m_ProjectionMatrixInverse);
		glm::mat4 m_ViewMatrixInverse = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.w),
			glm::vec3(m_Rotation.x, m_Rotation.y, m_Rotation.z));//这个实际上对于2d来说是没有必要的，只是我后面像尝试将透视矩阵和相机的一起写到这里面
		m_ViewMatrix = glm::inverse(m_ViewMatrixInverse);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

	}
}