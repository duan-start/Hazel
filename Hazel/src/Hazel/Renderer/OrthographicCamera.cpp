#include "hzpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Hazel {

	//第二次：
	//  正交/透视投影：接受世界的坐标（加上w分量写成齐次坐标的形式）
	//  输出四维向量，w分量计算实际是-Z,
	//  化为三维坐标的形式（NDC），（同时除以w分量,透视由来）
	//  对于opengl，ndcNear=-1,far=1;
	//  但是靠近前面的误差小一点，后面的误差大一点（1/Vz） 透视矫正（这个是）
	//  宽高比要和viewport的参数对应避免变形
	//  至于zbuffer,现代游戏引擎是使用wbuffer的

	//last:
	//glm::lookat(pos,dir,up)放回viewMatrix
	//获取矩阵的方式和设置的位置相反
	OrthographicCamera::OrthographicCamera(float Left, float Right, float Bottom, float Top) :m_ProjectionMatrix(glm::ortho(Left, Right, Bottom, Top,-1.0f,1.0f)), m_ViewMatrix(1.0f)
	{	
		HZ_PROFILE_FUNCTION();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}


	void OrthographicCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetForwardDir(const glm::vec3& forwardDir)
	{
		if (m_Rotation != glm::vec3(0.)) { HZ_CORE_INFO("Already Set CameraRotate"); }
		m_ForwardDir = forwardDir;
		m_ViewMatrix=glm::lookAt(m_ForwardDir,m_Position,m_UpDir);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetUpDir(const glm::vec3& upDir)
	{
		if (m_Rotation != glm::vec3(0.)) { HZ_CORE_INFO("Already Set CameraRotate"); }
		m_UpDir = upDir;
		m_ViewMatrix = glm::lookAt(m_ForwardDir, m_Position, m_UpDir);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(const glm::vec4& screen)
	{
		HZ_PROFILE_FUNCTION();
		m_ProjectionMatrix = glm::ortho(screen.x, screen.y, screen.z, screen.w);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	//2d固定渲染的坐标轴，3d使用glm::lookat
	void OrthographicCamera::RecalculateViewMatrix()
	{
		HZ_PROFILE_FUNCTION();
		//这里是设置相机的实际位置以及各种变换
		//inverse(viewInverse)就是 viewMatrix
		//所以顺序是先rotate然后translate
		//---------------------------------------------可能得考虑万向锁的事情了
		glm::mat4 m_ViewMatrixInverse = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z),
			glm::vec3(0., 0., 1.));
		//这个实际上对于2d来说是没有必要的，只是我后面像尝试将透视矩阵和相机的一起写到这里面
		m_ViewMatrix = glm::inverse(m_ViewMatrixInverse);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

	}
}