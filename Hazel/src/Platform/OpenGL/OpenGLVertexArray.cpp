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
		HZ_PROFILE_FUNCTION();
		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		HZ_PROFILE_FUNCTION();
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		HZ_PROFILE_FUNCTION();
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::UBind() const
	{
		HZ_PROFILE_FUNCTION();
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		HZ_PROFILE_FUNCTION();
		//��ȷ����layout����
		HZ_CORE_ASSERT(vertexBuffer->GetLayout().Get().size() != 0, "VertexBuffer has no layout");

		//����������Ҫ������bind����������ӱ�Ķ�������ϣ�����������̫ǿ
		glBindVertexArray(m_RendererID);
		//�����Լ��Ļ��Եø��Ӽ��
		vertexBuffer->Bind();

		//�Ҹ���һ��������������󶨶��vbo�����
		//
		uint32_t index = m_VertexBuffers.size();
		//const ��Ȼ������ǻ��bufferlayout�ĵ�����������һ��Ҫ��const���͵ķ��ص�����
		//�������begin ��end���������д�ģ���Ҫ�����ѭ�����õ������ģ���Ȼ�����˲�����Ϣ
		for (const auto& element : vertexBuffer->GetLayout()) {

			glEnableVertexArrayAttrib(m_RendererID, index);
			//�����int���͵Ļ����ͱ��뻻��һ������
			switch (element.Type) {
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glVertexAttribPointer(index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					vertexBuffer->GetLayout().GetStride(),
					(const void*)element.Offset);
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				//�������õĺ�������ͨ���õĺ����ǲ�һ����
				glVertexAttribIPointer(index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					vertexBuffer->GetLayout().GetStride(),
					(const void*)element.Offset);
				break;
			}
			//glVertexAttribPointer(index, element.GetComponentCount(),
			//	ShaderDataTypeToOpenGLBaseType(element.Type),
			//	element.Normalized ? GL_TRUE : GL_FALSE, vertexBuffer->GetLayout().GetStride(),
			//	(const void*)element.Offset);
			}
			index++;
			m_VertexBuffers.push_back(vertexBuffer);
		}
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		HZ_PROFILE_FUNCTION();
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}