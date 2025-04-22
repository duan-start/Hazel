#pragma once  
#include "Hazel/Core.h"  
#include "Hazel/Renderer/Shader.h"  

namespace Hazel {  
class OpenGLShader : public Shader  
{  
public:  
	OpenGLShader(const std::string& vertexsrc, const std::string& fragmentsrc);  
	virtual void Bind() const override;  
	virtual void UnBind() const override;  
	virtual ~OpenGLShader();  

	void UploadUniformBool(const std::string& name, bool value) const;  
	void UploadUniformInt(const std::string& name, int value) const;  
	void UploadUniformFloat(const std::string& name, float value) const;

	void UploadUniformMat4(const std::string& name, glm::mat4 value) const;  
	void UploadUniformVec2(const std::string& name, glm::vec2 value) const;  
	void UploadUniformVec3(const std::string& name, glm::vec3 value) const;  

private:  
	uint32_t m_RenderID;  
};  
}
