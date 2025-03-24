#pragma once
#include "Hazel/Core.h"
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Hazel {
	//û���Զ�ȫ����Ӷ��壬����һ�����ѵĲ��������
	class Shader
	{
	public:
		Shader(const std::string& vertexsrc, const std::string& fragmentsrc);
		void Bind()const;
		void Unbind()const;
		~Shader();

		//͵���Լ������Ŀ�����
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int  value) const;
		void setFloat(const std::string& name, float value) const;
		void setMatrix4fv(const std::string& name, glm::mat4 value) const;
		void setVec2(const std::string& name, glm::vec2 value) const;
		void setVec3(const std::string& name, glm::vec3 value) const;

	private:
		 uint32_t m_RenderID;
	};
}


