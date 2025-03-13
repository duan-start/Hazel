#pragma once

namespace Hazel {
class VerTexBuffer
{
public:
	virtual ~VerTexBuffer();
	virtual	void Bind()const = 0;
	virtual void UBind()const = 0;
	static VerTexBuffer* Creat(float* vertices,uint32_t size);
};

class IndexBuffer
{
public:
	virtual ~IndexBuffer();
	virtual	void Bind()const = 0;
	virtual void UBind()const = 0;
	static IndexBuffer* Creat(uint32_t* indices, uint32_t count);
};

}


