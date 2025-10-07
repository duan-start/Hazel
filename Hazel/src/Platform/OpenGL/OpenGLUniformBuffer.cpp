#include "hzpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Hazel {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		//����uniformbuffer ,���ô�С�����ð󶨵�shader�ӿ�
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		//�ϴ�����
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}
}