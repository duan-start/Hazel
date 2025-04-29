#pragma once
#include "Hazel/Renderer/Texture.h"

namespace Hazel {
	class OpenGLTexture2D:public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D();
		virtual uint32_t GetWidth()const { return m_Width; }
		virtual uint32_t GetHeight()const { return m_Height; }

		virtual void Bind(uint32_t slot=0)const;
	private:
		uint32_t m_Width, m_Height, m_RendererID;
		std::string m_path;
	};

}

