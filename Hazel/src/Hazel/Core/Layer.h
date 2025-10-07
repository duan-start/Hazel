#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Core/Timestep.h"
namespace Hazel {
	class HAZEL_API  Layer
	{
	public:
		//ÿһ�������
		//����˵��ÿ�����Լ�����ã���������ϻ����ǿյ�
		Layer(const std::string& debug_name = "Layer");
		virtual ~Layer();
		//ʵ��������������Դ��һ����ʼ��
		virtual void OnAttach();
		virtual void OnDetach();
		//������Ϸ�߼�����������
		virtual void OnUpdate(Timestep ts);
		//���Ƶ��Խ��� / �༭������
		virtual void OnImGuiRender();
		//�¼��Ĵ���
		virtual void OnEvent(Event& event);
		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};


}


