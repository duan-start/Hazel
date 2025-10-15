#pragma once


namespace Hazel {
	//渲染命令的绘制队列
	class  RenderCommandQueue
	{
	public:
		//声明函数指针类型
		typedef void(*RenderCommandFn)(void*);
		RenderCommandQueue();
		~RenderCommandQueue();

		//分配渲染命令及其数据
		//函数指针  int(数据大小) 具体数据
		void* Allocate(RenderCommandFn func, unsigned int size);

		//执行缓冲区内的命令
		void Execute();
	private:
		//字节为单位灵活存放任意数据结构。
		//“命令”和“命令对应的数据”
		std::byte* m_CommandBuffer;
		//实时指向的命令数据
		std::byte* m_CommandBufferPtr;
		unsigned int m_CommandCount = 0;

	};
}

