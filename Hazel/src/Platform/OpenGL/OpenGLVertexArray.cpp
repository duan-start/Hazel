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
		//先确定有layout布局
		HZ_CORE_ASSERT(vertexBuffer->GetLayout().Get().size() != 0, "VertexBuffer has no layout");

		//由于我们需要后续在bind函数里面添加别的东西，不希望这里耦合性太强
		glBindVertexArray(m_RendererID);
		//调用自己的会显得更加简洁
		vertexBuffer->Bind();

		//我改了一下这里，方便真正绑定多个vbo的情况
		//
		uint32_t index = m_VertexBuffers.size();
		//const ，然后后面是获得bufferlayout的迭代器，所以一定要有const类型的返回迭代器
		//所以这个begin 和end还不是随便写的，主要是这个循环是用迭代器的，虽然隐藏了部分信息
		for (const auto& element : vertexBuffer->GetLayout()) {

			glEnableVertexArrayAttrib(m_RendererID, index);
			//如果是int类型的话，就必须换有一个函数
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
				//整数设置的函数和普通设置的函数是不一样的
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