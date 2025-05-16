#include "hzpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include "glad/glad.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		if (&source == nullptr) {
			std::cout << "FAILED TO OPEN SHADER SOURCE";
		}
		auto shaderSources = Process(source);
		Compile(shaderSources);

		//根据文件的路径，提取对应的文件名字
		//assert/shader/texture.glsl
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind(".");
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}



	GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		HZ_CORE_ASSERT(false, "Unknown Shader Type");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& name,const std::string& vertexsrc, const std::string& fragmentsrc)
	{// Read our shaders into the appropriate buffers
		//我是真不愿写这么多
		std::unordered_map<GLenum, std::string> shaderSource;
		shaderSource[GL_VERTEX_SHADER] = vertexsrc;
		shaderSource[GL_FRAGMENT_SHADER] = fragmentsrc;
		Compile(shaderSource);
	}


	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in, std::ios::binary);

		if (in) {
			//查找末尾，偏移量为0
			in.seekg(0, std::ios::end);
			//利用当前的位置resize()
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			//全部读取
			in.read(&result[0], result.size());
			in.close();

		}
		else { HZ_CORE_ERROR("Failed To Open FilePath '{0}' ", filepath); }
		return  result;
	}

	//TODO: bind the shaderID with source code
	std::unordered_map<GLenum, std::string> OpenGLShader::Process(const std::string& source)
	{
		/*1.	typeToken 的硬编码问题：
?	typeToken 被硬编码为 "#type vertex"，这意味着代码只能处理一种类型的着色器。如果需要支持其他类型（如 fragment），需要额外处理。
2.	source.substr 的边界问题：
?	source.substr(begin, eol - begin) 假设 eol 总是大于 begin，但如果 typeToken 的格式不正确，可能会导致 eol - begin 为负值，从而引发运行时错误。
3.	find_first_not_of 和 find 的返回值未验证：
?	nextLinePos 和 pos 的返回值未检查是否为 std::string::npos，这可能导致后续的 substr 操作失败。
4.	ShaderTypeFromString 的返回值未验证：
?	ShaderTypeFromString(type) 的返回值直接用作键值插入 shaderSources，如果返回值无效（如 0），可能会导致逻辑错误。
5.	source.substr 的范围计算问题：
?	source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos)) 的逻辑复杂且容易出错，尤其是当 pos 为 std::string::npos 时。
		*/

		std::unordered_map<GLenum, std::string> shaderSources;
		const std::string typeToken = "#type ";
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos) {
			// 找到行尾
			size_t eol = source.find_first_of("\r\n", pos);
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error: Missing end of line after shader type declaration");

			// 提取类型字符串
			size_t begin = pos + typeToken.length();
			std::string type = source.substr(begin, eol - begin);
			HZ_CORE_ASSERT(!type.empty(), "Shader type is empty");
			GLenum shaderType = ShaderTypeFromString(type);
			HZ_CORE_ASSERT(shaderType != 0, "Invalid shader type specified");

			// 找到下一个非空行
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			HZ_CORE_ASSERT(nextLinePos != std::string::npos, "Shader source code is missing after type declaration");

			// 找到下一个类型标记
			pos = source.find(typeToken, nextLinePos);

			// 提取当前着色器代码
			size_t shaderCodeLength = (pos == std::string::npos) ? source.size() - nextLinePos : pos - nextLinePos;
			std::string shaderCode = source.substr(nextLinePos, shaderCodeLength);
			HZ_CORE_ASSERT(!shaderCode.empty(), "Shader code is empty");

			// 存储到 map 中
			shaderSources[shaderType] = shaderCode;
		}

		return shaderSources;
	}

	//TODU：使用源代码进行编译操作
	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSource)
	{
		HZ_CORE_ASSERT(shaderSource.size() == 2, "Support only 2 shaders now");
		GLuint program = glCreateProgram();

		//开始初始化的时候保留的应该是存储的最大值，而不简单是容量
		//std::vector<GLenum> glShaderIDs(shaderSource.size());
	//----------------------------------------
		//std::vector<GLenum> glShaderIDs;
		// glShaderIDs.reserve(shaderSource.size());
		//由于glenum 是一个unsigned int 类型的数值，且大小知晓，可以通过栈上分配的方式。
		std::array<GLenum, 2> glShaderIDs;
		//设置保留容量，即推入数据的时候不会去多次申请内存
		//用来计数
		unsigned int glShaderIDIndex = 0;
		m_RenderID = program;
		for (auto vf : shaderSource) {
			GLenum type = vf.first;
			const std::string& source = vf.second;
		// Create an empty vertex shader handle
		GLuint shader = glCreateShader(type);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* sourceCstr = (const GLchar*)source.c_str();
		glShaderSource(shader, 1, &sourceCstr, 0);

		// Compile the vertex shader
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(shader);

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_ASSERT(false, "Failed to init shader");
			break;
			}

		// Attach our shaders to our program
		 glAttachShader(program, shader);
		 glShaderIDs[glShaderIDIndex++]=shader;
		
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.

			for (auto id : glShaderIDs) {
			glDeleteShader(id);
			}

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_ASSERT(false, "Failed to link program");
			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs) {
			glDetachShader(program, id);
		}
		
		
	}

//---------------------------------------------------------
//---------------------------------------------------------
	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RenderID);
	}

	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RenderID);
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& value) const
	{
		UploadUniformVec4(name, value);
	}

	void OpenGLShader::SetUniformMat4(const std::string& name,const glm::mat4& value) const
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name,const glm::vec3& value) const
	{
		UploadUniformVec3(name, value);
	}

	void OpenGLShader::SetUniformInt(const std::string& name, int value) const
	{
		UploadUniformInt(name, value);
	}

	//---------------------------------------------------------
	//---------------------------------------------------------
	void OpenGLShader::UploadUniformBool(const std::string& name, const bool& value) const {
		glUniform1i(glGetUniformLocation(m_RenderID, name.c_str()), (int)value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, const int& value) const {
		glUniform1i(glGetUniformLocation(m_RenderID, name.c_str()), value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, const float& value) const {
		glUniform1f(glGetUniformLocation(m_RenderID, name.c_str()), value);
	}

	void OpenGLShader::UploadUniformVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y);
	}

	void OpenGLShader::UploadUniformVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformVec4(const std::string& name,const glm::vec4& value) const 
	{
		glUniform4f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y, value.z,value.w);
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& value) const {

		glUniformMatrix4fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name,const glm::mat3& value) const
	{
		glUniformMatrix3fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}


}