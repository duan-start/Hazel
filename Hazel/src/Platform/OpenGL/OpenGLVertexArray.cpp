#include "hzpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Hazel {
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Bool:		return GL_BOOL;
		}
		HZ_CORE_ASSERT(false, "Unknowen ShaderDataType");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::UBind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{

		//��ȷ����layout����
        HZ_CORE_ASSERT(vertexBuffer->GetLayout().Get().size() != 0, "VertexBuffer has no layout");
		
		//����������Ҫ������bind����������ӱ�Ķ�������ϣ�����������̫ǿ
		glBindVertexArray(m_RendererID);
		//�����Լ��Ļ��Եø��Ӽ��
		vertexBuffer->Bind();

		//�Ҹ���һ��������������󶨶��vbo�����
		uint32_t index = m_VertexBuffers.size();
		//const ��Ȼ������ǻ��bufferlayout�ĵ�����������һ��Ҫ��const���͵ķ��ص�����
		//�������begin ��end���������д�ģ���Ҫ�����ѭ�����õ������ģ���Ȼ�����˲�����Ϣ
		for (const auto& element : vertexBuffer->GetLayout()) {
			glEnableVertexArrayAttrib(m_RendererID, index);
			glVertexAttribPointer(index, element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE, vertexBuffer->GetLayout().GetStride(),
				(const void*)element.Offset);
			index++;
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}