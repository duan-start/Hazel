#include "hzpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Hazel {


	//这里简单介绍一下各个矩阵的作用，不然容易混：
	//这里的projectionMatrix作用在世界坐标上，返回的是一个四维的向量（w分量为1）
	//由于没有透视，所以只需要取其中x,y坐标映射到屏幕上，z用来做zbuffer;
	
	//第二次：
	//  正交/透视投影：接受世界的坐标（加上w分量写成齐次坐标的形式）
	//  得到对应的一个四维的向量，这个四维向量的w分量计算实际是-Vz,所以说透视是这么来的
	//  化为三维坐标的形式（NDC），（同时除以w分量）
	//  通过我的计算，对于opengl是满足前面到前面，后面到后面的映射的(是有点奇妙的)
	//  但是靠近前面的误差小一点，后面的误差大一点（1/Vz）
	//  至于左右和上下就要和viewport的参数对应避免变形
	//  至于zbuffer,现代游戏引擎是使用wbuffer的
	//  NDC的插值不能直接，要/Vz;
	OrthographicCamera::OrthographicCamera(float Left, float Right, float Bottom, float Top) :m_ProjectionMatrix(glm::ortho(Left, Right, Bottom, Top,-1.0f,1.0f)), m_ViewMatrix(1.0f)
	{	//near = 0.1f → 实际表示近平面在 z = -0.1f 的位置。
		//far = 100.0f → 表示远平面在 z = -100.0f 的位置。

		HZ_PROFILE_FUNCTION();
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
		HZ_PROFILE_FUNCTION();
		m_ProjectionMatrix = glm::ortho(screen.x, screen.y, screen.z, screen.w);
		//直接这样计算就好了，没有必要调用recal。。函数
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	//简单的思路：根据相机的位置和旋转角度（如果是2d的话就是一个一维的值）（这个是使用者给定的），去计算从原点到这样的矩阵（从右往左的顺序，先旋转后移动），然后求个逆，视图矩阵就写好了
	//其实这是2d简化版本的，如果是3d的话，相机的坐标是一个维度为3的向量，相机的方向也是一个维度为3的向量
	void OrthographicCamera::RecalculateViewMatrix()
	{
		HZ_PROFILE_FUNCTION();
		// 正确的2D相机矩阵顺序：先旋转后平移（T * R）的逆 = R^-1 * T^-1
		//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.w),
		//	glm::vec3(0.0f, 0.0f, 1.0f)); // 强制Z轴
		//glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);

		//// 视图矩阵 = (T * R)^-1 = R^-1 * T^-1
		//m_ViewMatrix = glm::transpose(rotation) * glm::translate(glm::mat4(1.0f), -m_Position);


		//m_ViewProjection=glm::inverse(m_ViewMatrixInverse*m_ProjectionMatrixInverse);

		//旋转会变形的根本原因是所有的点是相对于坐标原点的旋转，会变形
		//如果只能绕z轴旋转的话，我们需要先将对应的图形的原点平移至原点，然后绘制
		glm::mat4 m_ViewMatrixInverse = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.w),
			glm::vec3(m_Rotation.x, m_Rotation.y, m_Rotation.z));//这个实际上对于2d来说是没有必要的，只是我后面像尝试将透视矩阵和相机的一起写到这里面
		m_ViewMatrix = glm::inverse(m_ViewMatrixInverse);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

	}
}