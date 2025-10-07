#pragma once
#include "Hazel/Core/Core.h"

namespace Hazel {
	//专门从cpu端到gpu端的，有binding(这是vulkan的资源绑定的方式，非常精确又好)
	class UniformBuffer
	{
	public:
		UniformBuffer()=default;
		virtual ~UniformBuffer()=default;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset) = 0;
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};
}

