#pragma once
#include "Hazel/Core.h"
#include <string>

namespace Hazel {
	//û���Զ�ȫ����Ӷ��壬����һ�����ѵĲ��������
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


