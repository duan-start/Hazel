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
		//保存引用
		//cpu端的handel
		uint32_t m_RendererID;

		//由于opengl状态机的原因，在添加关系时我们必须立即绑定其他的
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}


