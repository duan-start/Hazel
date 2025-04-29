#include "hzpch.h"  
#include "Texture.h"  
#include "Renderer.h"  

#include "Platform/OpenGL/OpenGLTexture2D.h" // 修复了文件扩展名缺失的问题  

namespace Hazel {  

Ref<Texture2D> Hazel::Texture2D::Create(const std::string& path)  
{  
	switch (RendererAPI::GetAPI()) {  
	case RendererAPI::API::None: HZ_CORE_ASSERT(false, "UnKnow RendererAPI"); break;  
	case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path); break;  

	}  
	HZ_CORE_ASSERT(false,"UnKnow RendererAPI");  
	return nullptr;  
}  
}
