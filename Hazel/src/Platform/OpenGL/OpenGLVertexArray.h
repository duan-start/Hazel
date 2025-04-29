#pragma once

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {
	class OpenGLVertexArray:public VerTexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();
		virtual	void Bind()const  override;
		virtual void UBind()const override;
		virtual void  AddVertexBuffer(const Ref<VerTexBuffer>& vertexBuffer) override;
		virtual void  SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)override;

		virtual const std::vector<Ref<VerTexBuffer>>& GetVertexBuffers() const override {
			return m_VertexBuffers;
		};
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override {
			return m_IndexBuffer;
		};
	private:
		//±£¥Ê“˝”√
		uint32_t m_RendererID;
		std::vector<Ref<VerTexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}


