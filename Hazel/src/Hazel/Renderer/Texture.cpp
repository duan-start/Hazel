#include "hzpch.h"  
#include "Texture.h"  
#include "Renderer3D.h"  

#include "Platform/OpenGL/OpenGLTexture2D.h" 
#include "Platform/OpenGL/OpenGLTextureCube.h"
namespace Hazel {
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "UnKnow RendererAPI"); break;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width,height); break;
		}
		HZ_CORE_ASSERT(false, "UnKnow RendererAPI");
		return nullptr;
	}

	Ref<Texture2D> Hazel::Texture2D::Create(const std::string& path)
{  
	switch (RendererAPI::GetAPI()) {  
	case RendererAPI::API::None: HZ_CORE_ASSERT(false, "UnKnow RendererAPI"); break;  
	case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path); break;  

	}  
	HZ_CORE_ASSERT(false,"UnKnow RendererAPI");  
	return nullptr;  
}  



	void TextureLibrary::Add(const std::string& name, const Ref<Texture>& texture)
	{
		m_Textures[name] = texture;
	}

	Ref<Texture> TextureLibrary::Get(const std::string& name)
	{
		HZ_CORE_ASSERT(Exists(name), "Shader does not exists");
		return m_Textures[name];
	}


	void TextureLibrary::Load(const std::string& filepath)
	{
		Ref<Texture> texture = Texture2D::Create(filepath);
		auto& name = texture->GetName();
		HZ_CORE_ASSERT(!Exists(name), "Shader has been already exists");
		Add(name,texture);
	}

	bool TextureLibrary::Exists(const std::string& name) const
	{
		return  m_Textures.find(name) != m_Textures.end();
	}
	Ref<TextureCube> TextureCube::Create(const std::string& path)
	{
		switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "UnKnow RendererAPI"); break;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTextureCube>(path); break;

		}
		HZ_CORE_ASSERT(false, "UnKnow RendererAPI");
		return nullptr;
	}
}
