#pragma once
#include "Hazel/Core/Core.h"

namespace Hazel {
	
	class UniformBuffer
	{
	public:
		UniformBuffer()=default;
		virtual ~UniformBuffer()=default;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset) = 0;
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};
}

