#pragma once
#include <memory>
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {
	class VerTexArray
	{
	public:
		virtual ~VerTexArray();
		virtual	void Bind()const = 0;
		virtual void UBind()const = 0;
		virtual void  AddVertexBuffer(const Ref<VerTexBuffer>& vertexBuffer)  = 0;
		virtual void  SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)  = 0;
		static Ref<VerTexArray> Create();

		virtual const std::vector<Ref<VerTexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

	};

}