#pragma once  

#include "Hazel/Core/Core.h"  
#include "Hazel/Renderer/Shader.h"  

typedef unsigned int GLenum;

namespace Hazel {  
class OpenGLShader : public Shader  
{  
public:  
	static GLenum ShaderTypeFromString(const std::string& type);

	OpenGLShader(const std::string& name,const std::string& vertexsrc, const std::string& fragmentsrc); 
	OpenGLShader(const std::string& filepath);

	
	virtual const std::string& GetName()const override { return m_Name; }
	virtual void Bind() const override;  
	virtual void UnBind() const override;  
	virtual ~OpenGLShader();  

	virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value)const override;
	virtual void SetUniformMat4(const std::string& name, const glm::mat4& value)const override;
	virtual void SetUniformFloat3(const std::string& name,const glm::vec3& value)const override;
	void SetUniformFloat(const std::string& name, float value) const override;

	virtual void SetUniformIntArray(const std::string& name,int* value, uint32_t count)  override;
	virtual void SetUniformInt(const std::string& name, int value) const override;

	//这三种类型直接复制也没有关系
	void UploadUniformBool(const std::string& name,const bool& value) const;
	void UploadUniformInt(const std::string& name, const int& value) const;
	void UploadUniformFloat(const std::string& name, const float& value)const;
	void UploadUniformIntArray(const std::string& name, const int* value, uint32_t count);

	void UploadUniformMat4(const std::string& name, const glm::mat4& value) const;
	void UploadUniformMat3(const std::string& name, const glm::mat3& value) const;

	void UploadUniformVec2(const std::string& name, const glm::vec2& value) const;
	void UploadUniformVec3(const std::string& name, const glm::vec3& value) const;
	void UploadUniformVec4(const std::string& name, const glm::vec4& value) const ;
	//由于两个构造函数有共用的部分，所以这边要封装成一个私有的函数
private:
	
	std::string ReadFile(const std::string& filepath);

	std::unordered_map<GLenum,std::string> Process(const std::string& source);
	void Compile(const std::unordered_map<GLenum, std::string>& shaderSource);
private:  
	uint32_t m_RenderID;  
	std::string m_Name;
};  
}
