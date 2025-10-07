#pragma once

#include <glm/glm.hpp>

namespace Hazel {
	//������ʱ�Ͳ����ˣ��������3dϵͳ��ʱ����԰�����һ����ϣ�
	//����������࣬������͸��
	enum class CameraAPI {
		None=0,Ortho=1,Prspective=2,
	};
	// ��������ͬ������Ҳû�õ�
	//����������Ĵ����ǽ�gpu���ϵ��ظ�����ת��cpu��һ�μ���
	//��Ҫ��ViewprojectionMatrix����ļ��㣬Ȼ���ϴ���GPU
	class Camera
	{
	public:
		//positon x,y,z
		virtual const glm::vec3& GetPosition()const = 0;
		//vec4 ��Ԫ����û������������vec3 (yaw,pitch,roll)����������
		//�ҵ����ʵ��������ĳ������ת���ٶ���
		virtual const glm::vec4& GetRotation()const = 0;

		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual void SetRotation(const glm::vec4& rotation)  = 0;
		//������Ļ���ĸ�����������mat4*4��projection����
		virtual void SetProjection(const glm::vec4& screen) = 0;

		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;

		static Camera* Create(float Left,float Right,float Bottom,float Top);

	protected:
		virtual void RecalculateViewMatrix()  = 0;
	private:
		//���õ�����
		static CameraAPI s_CamerType;
	};
}


