#pragma once
#include "Hazel/Core.h"
#include <string>

namespace Hazel {
	//没有自动全部添加定义，那是一个付费的插件好像是
	class Shader
	{
	public:
		Shader(const std::string& vertexsrc, const std::string& fragmentsrc);
		void Bind()const;
		void Unbind()const;
		~Shader();
	private:
		 uint32_t m_RenderID;
	};
}


