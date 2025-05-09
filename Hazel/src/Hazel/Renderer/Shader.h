#pragma once
#include "Hazel/Core/Core.h"

#include <string>
#include <glm/glm.hpp>



namespace Hazel {
	//没有自动全部添加定义，那是一个付费的插件好像是
	class Shader
	{
	public:
		Shader() {};
		virtual void Bind() const = 0;  // 纯虚函数，声明为const
		virtual void UnBind() const = 0;  // 纯虚函数，声明为const
		virtual const std::string& GetName()const = 0;

		virtual ~Shader() ;
		static Ref<Shader> Create(const std::string& name,const std::string& vertexsrc, const std::string& fragmentsrc);

		static Ref<Shader> Create(const std::string& filepath);

	};

//创建一个ShaderLibrary ，用来保存引擎原始的基本的shader 的效果，方便使用。
// 这里面以后可以扩展放一些新的图形学的渲染的一些方法
	class ShaderLibrary {
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Get(const std::string& name);

		void Load(const std::string& name, const std::string& sourceCode,const std::string& fragment);
		void Load(const std::string& filePath);

		bool Exists(const std::string& name) const;

	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}

