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
		virtual void  AddVertexBuffer(const std::shared_ptr<VerTexBuffer>& vertexBuffer)  = 0;
		virtual void  SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)  = 0;
		static VerTexArray* Create();

		virtual const std::vector<std::shared_ptr<VerTexBuffer>>& GetVertexBuffers() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

	};

}