#include "hzpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Hazel {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		//创建uniformbuffer ,设置大小，设置绑定的shader接口
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
		//上传数据
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}
}