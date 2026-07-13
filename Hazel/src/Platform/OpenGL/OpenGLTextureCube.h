#pragma once
#include "Hazel/Renderer/Texture.h"

namespace Hazel {
	class OpenGLTextureCube:public TextureCube
	{
	public:
		OpenGLTextureCube(const std::string& path);
		virtual ~OpenGLTextureCube();

		virtual void Bind(unsigned int slot = 0) const override;

		virtual TextureFormat GetFormat() const { return m_Format; }
		virtual unsigned int GetWidth() const override{ return m_Width; }
		virtual unsigned int GetHeight() const override { return m_Height; }

		virtual const std::string& GetPath() const override { return m_FilePath; }
		virtual uint32_t GetRenderID()const { return m_RendererID; };

		//Todo
		virtual void SetData(void*, uint32_t size) {};
		virtual bool operator==(const Texture& other) const { return false; };

		virtual bool IsLoaded()const { return false; };
		virtual const std::string& GetName()const { return std::string(); };
	private:
		uint32_t m_RendererID;
		TextureFormat m_Format;
		unsigned int m_Width, m_Height;

		unsigned char* m_ImageData;

		std::string m_FilePath;
	};
}


