#pragma once
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {
	class OpenGLVerTexBuffer:public VerTexBuffer
	{
	public:
		OpenGLVerTexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVerTexBuffer();
		virtual	void Bind()const ;
		virtual void UBind()const ;
	private :
		uint32_t m_RenderID;
	};

	class OpenGLIndexBuffer :public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();
		virtual	void Bind()const ;
		virtual void UBind()const ;
	private:
		uint32_t m_RenderID;
	};
}

