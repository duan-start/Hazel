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
		virtual void  AddVertexBuffer(const std::shared_ptr<VerTexBuffer>& vertexBuffer) override;
		virtual void  SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)override;

		virtual const std::vector<std::shared_ptr<VerTexBuffer>>& GetVertexBuffers() const override {
			return m_VertexBuffers;
		};
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override {
			return m_IndexBuffer;
		};
	private:
		//±£¥Ê“˝”√
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VerTexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}


