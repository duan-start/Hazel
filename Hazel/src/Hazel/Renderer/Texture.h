#pragma once
#include "Hazel/Core/Core.h"
namespace Hazel {
	class Texture
	{
	public:
		virtual uint32_t GetWidth()const = 0;
		virtual uint32_t GetHeight()const = 0;
		virtual void SetData(void*, uint32_t size) = 0;
		virtual bool operator==(const Texture& other) const = 0;
		virtual uint32_t GetRenderID()const = 0;

		virtual void Bind(uint32_t slot = 0)const = 0;
		virtual ~Texture() = default;
	};

	class Texture2D : public Texture {
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
	};
}

