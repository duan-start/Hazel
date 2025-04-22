#pragma once
#include "Hazel/Core.h"

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
		virtual ~Shader() ;
		static Shader* Create(const std::string& vertexsrc, const std::string& fragmentsrc);
	};
}


