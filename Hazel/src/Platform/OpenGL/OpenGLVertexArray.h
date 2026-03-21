#pragma once

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {
	class OpenGLVertexArray:public VertexArray
	{
	public:
		//一个vao是用来解释vbo的数据，可以绑定多个vbo或者一个vbo。这个解释一旦确定了就不能改了
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();
		virtual	void Bind()const  override;
		virtual void UBind()const override;
		virtual void  AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void  SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override {
			return m_VertexBuffers;
		};
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override {
			return m_IndexBuffer;
		};
	private:
		//handleId
		uint32_t m_RendererID;

		//vao实际是用来引用这里面的数据（多个vbo个一个ibo）
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		int m_AttributeIndex{};

		Ref<IndexBuffer> m_IndexBuffer;
	};
}


