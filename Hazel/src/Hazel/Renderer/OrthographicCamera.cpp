#include "hzpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Hazel {


	//����򵥽���һ�¸�����������ã���Ȼ���׻죺
	//�����projectionMatrix���������������ϣ����ص���һ����ά��������w����Ϊ1��
	//����û��͸�ӣ�����ֻ��Ҫȡ����x,y����ӳ�䵽��Ļ�ϣ�z������zbuffer;
	//��������һ�û��̫������
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
		m_Rotation = glm::vec4(rotation.x, rotation.y, rotation.z, rotation.w); // Ӳ����Z��
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetProjection(const glm::vec4& screen)
	{
		m_ProjectionMatrix = glm::ortho(screen.x, screen.y, screen.z, screen.w);
		//ֱ����������ͺ��ˣ�û�б�Ҫ����recal��������
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	//�򵥵�˼·�����������λ�ú���ת�Ƕȣ������2d�Ļ�����һ��һά��ֵ���������ʹ���߸����ģ���ȥ�����ԭ�㵽�����ľ��󣨴��������˳������ת���ƶ�����Ȼ������棬��ͼ�����д����
	//��ʵ����2d�򻯰汾�ģ������3d�Ļ��������������һ��ά��Ϊ3������������ķ���Ҳ��һ��ά��Ϊ3������
	void OrthographicCamera::RecalculateViewMatrix()
	{
		// ��ȷ��2D�������˳������ת��ƽ�ƣ�T * R������ = R^-1 * T^-1
		//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.w),
		//	glm::vec3(0.0f, 0.0f, 1.0f)); // ǿ��Z��
		//glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);

		//// ��ͼ���� = (T * R)^-1 = R^-1 * T^-1
		//m_ViewMatrix = glm::transpose(rotation) * glm::translate(glm::mat4(1.0f), -m_Position);


		//m_ViewProjection=glm::inverse(m_ViewMatrixInverse*m_ProjectionMatrixInverse);
		glm::mat4 m_ViewMatrixInverse = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.w),
			glm::vec3(m_Rotation.x, m_Rotation.y, m_Rotation.z));//���ʵ���϶���2d��˵��û�б�Ҫ�ģ�ֻ���Һ������Խ�͸�Ӿ���������һ��д��������
		m_ViewMatrix = glm::inverse(m_ViewMatrixInverse);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

	}
}