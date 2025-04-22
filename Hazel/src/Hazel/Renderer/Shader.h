#pragma once
#include "Hazel/Core.h"

#include <string>
#include <glm/glm.hpp>


namespace Hazel {
	//û���Զ�ȫ����Ӷ��壬����һ�����ѵĲ��������
	class Shader
	{
	public:
		Shader() {};
		virtual void Bind() const = 0;  // ���麯��������Ϊconst
		virtual void UnBind() const = 0;  // ���麯��������Ϊconst
		virtual ~Shader() ;
		static Shader* Create(const std::string& vertexsrc, const std::string& fragmentsrc);
	};
}


