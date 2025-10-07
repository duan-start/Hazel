#pragma once
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {
	class OpenGLVertexBuffer:public VertexBuffer
	{
	public:

		OpenGLVertexBuffer(float* vertices, uint32_t size);
		
		//batch Rendering
		//只是设置状态绑定（设置顶点属性和预留的位置），没有具体的数据
		OpenGLVertexBuffer(uint32_t size);

		virtual ~OpenGLVertexBuffer();
		virtual	void Bind()const  override ;
		virtual void UBind()const override ;

		virtual void SetData(const void* data, uint32_t size) override;
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private :
		uint32_t m_RenderID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer :public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();
		virtual	void Bind()const override;
		virtual void UBind()const override;
		virtual uint32_t GetCount() const override;
	private:
		uint32_t m_RenderID;
		uint32_t m_Count;
	};
}

