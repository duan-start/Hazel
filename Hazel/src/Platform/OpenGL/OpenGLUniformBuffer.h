#pragma once
#include "Hazel/Renderer/UniformBuffer.h"

namespace Hazel {

	class OpenGLUniformBuffer:public UniformBuffer
	{
	public:
		//size	UBO 数据大小，单位字节
		//binding	UBO 绑定点，和 GLSL layout(binding = X) 对应
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();
		//从cpu端向gpu端进行数据传输（void* 传输size到gpuBuffer里面偏移offset里面）
		virtual void SetData(const void* data, uint32_t size, uint32_t offset)override;
	private:
		//m_RendererID是cpu 端创建的一个handle,在gpu上创建了对应的内存，需要用这个进行数据的传输
		uint32_t m_RendererID=0;
	};
}

