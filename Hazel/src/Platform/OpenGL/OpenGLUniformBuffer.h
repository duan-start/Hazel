#pragma once
#include "Hazel/Renderer/UniformBuffer.h"

namespace Hazel {

	class OpenGLUniformBuffer:public UniformBuffer
	{
	public:
		//size	UBO ���ݴ�С����λ�ֽ�
		//binding	UBO �󶨵㣬�� GLSL layout(binding = X) ��Ӧ
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();
		//��cpu����gpu�˽������ݴ��䣨void* ����size��gpuBuffer����ƫ��offset���棩
		virtual void SetData(const void* data, uint32_t size, uint32_t offset)override;
	private:
		//m_RendererID��cpu �˴�����һ��handle,��gpu�ϴ����˶�Ӧ���ڴ棬��Ҫ������������ݵĴ���
		uint32_t m_RendererID=0;
	};
}

