#include "hzpch.h"
#include "Mesh.h"
#include "Renderer3D.h"
#include <Platform/OpenGL/OpenGLMesh.h>


namespace Hazel {


	Ref<Mesh> Mesh::Create(const std::string& filename)
	{
		switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "UnKnow RendererAPI"); break;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLMesh>(filename); break;
		}
		HZ_CORE_ASSERT(false, "UnKnow RendererAPI");
		return nullptr;
	}

}