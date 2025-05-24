#pragma once
#include "Hazel/Renderer/Texture.h"

typedef  unsigned int GLenum;

namespace Hazel {
	class OpenGLTexture2D:public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(uint32_t width, uint32_t height);

		~OpenGLTexture2D();
		virtual uint32_t GetWidth()const { return m_Width; }
		virtual uint32_t GetHeight()const { return m_Height; }
		virtual bool operator==(const Texture& other) const override;

		virtual void SetData(void*, uint32_t size)override;

		virtual void Bind(uint32_t slot=0)const;
	private:
		uint32_t m_Width, m_Height, m_RendererID;
		std::string m_path;

		GLenum m_InternalFormat=0 , m_DataFormat=0 ;
		
	};

}

