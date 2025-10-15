#include "hzpch.h"
#include "RenderCommandQueue.h"

//可变参数宏，这是固定的
#define HZ_RENDER_TRACE(...) HZ_CORE_TRACE(__VA_ARGS__)

namespace Hazel {
	RenderCommandQueue::RenderCommandQueue()
	{
		//堆上创建并进行初始化
		m_CommandBuffer = new std::byte[10 * 1024 * 1024]; // 10mb buffer
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}

	RenderCommandQueue::~RenderCommandQueue()
	{
		delete[] m_CommandBuffer;
	}

	void* RenderCommandQueue::Allocate(RenderCommandFn fn, unsigned int size)
	{
		// ptr(address)->functiion adderss   (unsigned int)size  (memory adress)data

		// TODO: alignment
		*(RenderCommandFn*)m_CommandBufferPtr = fn;
		m_CommandBufferPtr += sizeof(RenderCommandFn);

		//指针指向的数据是size的数值
		*(unsigned int*)m_CommandBufferPtr = size;
		//unsigned int 和int的大小是一样的，一般是4个字节
		m_CommandBufferPtr += sizeof(unsigned int);


		//这里就不再是存数据的指针了，而是直接存数据
		//memory 是用来给外界写入数据的
		void* memory = m_CommandBufferPtr;
		m_CommandBufferPtr += size;

		m_CommandCount++;
		return memory;
	}

	//命令统一执行
	void RenderCommandQueue::Execute()
	{

		HZ_RENDER_TRACE("RenderCommandQueue::Execute -- {0} commands, {1} bytes", m_CommandCount, (m_CommandBufferPtr - m_CommandBuffer));

		std::byte* buffer = m_CommandBuffer;
		for (unsigned int i = 0; i < m_CommandCount; i++) {
			RenderCommandFn F = *(RenderCommandFn*)buffer;
			buffer += sizeof(RenderCommandFn);

			unsigned int size = *(unsigned int*)buffer;
			buffer += sizeof(unsigned int);
			//excute  void*
			F(buffer);
			buffer += size;
		}

		//Reset status
		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}

}