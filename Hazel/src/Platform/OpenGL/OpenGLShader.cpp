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
			std::cout << "funck";
		}
		auto shaderSources = Process(source);
		Compile(shaderSources);
	}



	GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		HZ_CORE_ASSERT(false, "Unknown Shader Type");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& vertexsrc, const std::string& fragmentsrc)
	{// Read our shaders into the appropriate buffers
		//�����治Ըд��ô��
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
			//����ĩβ��ƫ����Ϊ0
			in.seekg(0, std::ios::end);
			//���õ�ǰ��λ��resize()
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			//ȫ����ȡ
			in.read(&result[0], result.size());
			in.close();

		}
		else { HZ_CORE_ERROR("Failed To Open FilePath '{0}' ", filepath); }
		return  result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::Process(const std::string& source)
	{
		/*1.	typeToken ��Ӳ�������⣺
?	typeToken ��Ӳ����Ϊ "#type vertex"������ζ�Ŵ���ֻ�ܴ���һ�����͵���ɫ���������Ҫ֧���������ͣ��� fragment������Ҫ���⴦��
2.	source.substr �ı߽����⣺
?	source.substr(begin, eol - begin) ���� eol ���Ǵ��� begin������� typeToken �ĸ�ʽ����ȷ�����ܻᵼ�� eol - begin Ϊ��ֵ���Ӷ���������ʱ����
3.	find_first_not_of �� find �ķ���ֵδ��֤��
?	nextLinePos �� pos �ķ���ֵδ����Ƿ�Ϊ std::string::npos������ܵ��º����� substr ����ʧ�ܡ�
4.	ShaderTypeFromString �ķ���ֵδ��֤��
?	ShaderTypeFromString(type) �ķ���ֱֵ��������ֵ���� shaderSources���������ֵ��Ч���� 0�������ܻᵼ���߼�����
5.	source.substr �ķ�Χ�������⣺
?	source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos)) ���߼����������׳��������ǵ� pos Ϊ std::string::npos ʱ��
		*/

		std::unordered_map<GLenum, std::string> shaderSources;
		const std::string typeToken = "#type ";
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos) {
			// �ҵ���β
			size_t eol = source.find_first_of("\r\n", pos);
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error: Missing end of line after shader type declaration");

			// ��ȡ�����ַ���
			size_t begin = pos + typeToken.length();
			std::string type = source.substr(begin, eol - begin);
			HZ_CORE_ASSERT(!type.empty(), "Shader type is empty");
			GLenum shaderType = ShaderTypeFromString(type);
			HZ_CORE_ASSERT(shaderType != 0, "Invalid shader type specified");

			// �ҵ���һ���ǿ���
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			HZ_CORE_ASSERT(nextLinePos != std::string::npos, "Shader source code is missing after type declaration");

			// �ҵ���һ�����ͱ��
			pos = source.find(typeToken, nextLinePos);

			// ��ȡ��ǰ��ɫ������
			size_t shaderCodeLength = (pos == std::string::npos) ? source.size() - nextLinePos : pos - nextLinePos;
			std::string shaderCode = source.substr(nextLinePos, shaderCodeLength);
			HZ_CORE_ASSERT(!shaderCode.empty(), "Shader code is empty");

			// �洢�� map ��
			shaderSources[shaderType] = shaderCode;
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSource)
	{

		GLuint program = glCreateProgram();
		std::vector<GLenum> glShaderIDs(shaderSource.size());
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
		 glShaderIDs.push_back(shader);
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

	//---------------------------------------------------------
	//---------------------------------------------------------
	void OpenGLShader::UploadUniformBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(m_RenderID, name.c_str()), (int)value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(m_RenderID, name.c_str()), value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(m_RenderID, name.c_str()), value);
	}

	void OpenGLShader::UploadUniformVec2(const std::string& name, glm::vec2 value) const
	{
		glUniform2f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y);
	}

	void OpenGLShader::UploadUniformVec3(const std::string& name, glm::vec3 value) const
	{
		glUniform3f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformVec4(const std::string& name, glm::vec4 value) const
	{
		glUniform4f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y, value.z,value.w);
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, glm::mat4 value) const {

		glUniformMatrix4fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, glm::mat3 value) const
	{
		glUniformMatrix3fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}


}