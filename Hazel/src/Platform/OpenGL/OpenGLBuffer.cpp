#include "hzpch.h"
#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Hazel {
	OpenGLVerTexBuffer::OpenGLVerTexBuffer(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_RenderID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	OpenGLVerTexBuffer::~OpenGLVerTexBuffer()
	{
		glDeleteBuffers(1, &m_RenderID);
	}
	void OpenGLVerTexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	}
	void OpenGLVerTexBuffer::UBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}



	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
	{
		glCreateBuffers(1, &m_RenderID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RenderID);
	}
	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
	}
	void OpenGLIndexBuffer::UBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}