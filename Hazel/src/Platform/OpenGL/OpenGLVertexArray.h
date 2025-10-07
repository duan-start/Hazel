#pragma once

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {
	class OpenGLVertexArray:public VertexArray
	{
	public:
		//һ��vao����������vbo�����ݣ����԰󶨶��vbo����һ��vbo���������һ��ȷ���˾Ͳ��ܸ���
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
		//��������
		//cpu�˵�handel
		uint32_t m_RendererID;

		//����opengl״̬����ԭ������ӹ�ϵʱ���Ǳ���������������
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}


