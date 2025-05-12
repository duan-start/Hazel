#include "hzpch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace Hazel {
	Ref<Shader> Shader::Create(const std::string& name,const std::string& vertexsrc, const std::string& fragmentsrc) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI"); return nullptr;
		case RendererAPI::API::OpenGL: return  std::make_shared<OpenGLShader>(name,vertexsrc, fragmentsrc);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	};

	Ref<Shader> Shader::Create(const std::string& filepath) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "Do not support this RenderAPI"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filepath);
		}
		HZ_CORE_ASSERT(false, "Unknown RenderAPI"); return nullptr;
	};



	Shader::~Shader()
	{
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		//如果查找发现m_shader里面有shader
		HZ_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader has been already exists");

		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		HZ_CORE_ASSERT(Exists(name), "Shader does not exists");
		return m_Shaders[name];
	}

	//对于的同样的功能，我们应该提高单一接口的复用性
	void ShaderLibrary::Load(const std::string& name, const std::string& vertex,const std::string& fragment)
	{
		HZ_CORE_ASSERT(!Exists(name), "Shader has been already exists");
		Ref<Shader> shader= Shader::Create(name, vertex, fragment);
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(const std::string& filepath)
	{
		Ref<Shader> shader=Shader::Create(filepath);
		auto& name= shader->GetName();
		HZ_CORE_ASSERT(!Exists(name), "Shader has been already exists");
		//m_Shaders[name] =
		//m_Shaders[name] = shader;
		Add(shader);
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return  m_Shaders.find(name) != m_Shaders.end();
	}

}