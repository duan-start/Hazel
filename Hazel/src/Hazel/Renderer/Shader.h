#pragma once
#include "Hazel/Core/Core.h"

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
		virtual const std::string& GetName()const = 0;

		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value)const = 0;
		virtual void SetUniformMat4(const std::string& name,const glm::mat4& value)const = 0;
		virtual void SetUniformFloat3(const std::string& name,const glm::vec3& value)const=0 ;
		virtual void SetUniformFloat(const std::string& name, float value) const = 0;

		//�������ò������Ŀ���
		virtual void SetUniformInt(const std::string& name, int value)const = 0;

		virtual ~Shader() ;
		static Ref<Shader> Create(const std::string& name,const std::string& vertexsrc, const std::string& fragmentsrc);

		static Ref<Shader> Create(const std::string& filepath);

	};

//����һ��ShaderLibrary ��������������ԭʼ�Ļ�����shader ��Ч��������ʹ�á�
// �������Ժ������չ��һЩ�µ�ͼ��ѧ����Ⱦ��һЩ����
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

