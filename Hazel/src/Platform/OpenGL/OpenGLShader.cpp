#include "hzpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <filesystem>
#include "glad/glad.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Hazel/Core/Timer.h"



namespace Hazel {
	//��������ĺ���ȫ�������������
	namespace Utils {
		//Only Used By This File
		//stirng --��Ӧ��shader��GL_enum��������
		static GLenum ShaderTypeFromString(const std::string& type) {
			if (type == "vertex") return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

			HZ_CORE_ASSERT(false, "Unknown Shader Type");
			return 0;
		}

		//�����ļ���·����ȡ�����string stream
		static std::string ReadFile(const std::string& filepath) {
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
		
		//ѡ������Ϊ��Ӧ��shader�Ķ������ļ�����
		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			HZ_CORE_ASSERT(false,"UnKnowen Shader Stage");
			return (shaderc_shader_kind)0;
		}
		//stage--to const char*
		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			HZ_CORE_ASSERT(false,"UnKonwen Shader Stage");
			return nullptr;
		}
		//��ȡ�����Ŀ¼��������shader ������ŵ�λ��
		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}
		//��������Ŀ¼
		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}
		//���ú��ʵĺ�׺��չopengl
		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			HZ_CORE_ASSERT(false,"UnKonwen Shader Stage");
			return "";
		}
		//���ú��ʵĺ�׺��չvulkan
		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			HZ_CORE_ASSERT(false,"UnKonwen Shader Stage");
			return "";
		}

	}
	//
	OpenGLShader::OpenGLShader(const std::string& filepath):m_FilePath(filepath)
	{
		HZ_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		auto shaderSources = Process(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			HZ_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}


	//
	GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		HZ_PROFILE_FUNCTION();
		return Utils::ShaderTypeFromString(type);
	}
	//
	OpenGLShader::OpenGLShader(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		HZ_PROFILE_FUNCTION();
		// Read our shaders into the appropriate buffers
		//�����治Ըд��ô��
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		
		CompileOrGetVulkanBinaries(sources);
		CompileOrGetOpenGLBinaries();
		CreateProgram();
	}


	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		HZ_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				HZ_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else
		{
			HZ_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	//TODO: bind the shaderID with source code
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

		HZ_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;
		

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			HZ_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			HZ_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}


		return shaderSources;
	}

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{

		GLuint program = glCreateProgram();
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);
				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
			if (module.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				HZ_CORE_ERROR(module.GetErrorMessage());
				HZ_CORE_ASSERT(false,"");
			}

			shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

			std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
			if (out.is_open())
			{
				auto& data = shaderData[stage];
				out.write((char*)data.data(), data.size() * sizeof(uint32_t));
				out.flush();
				out.close();
			}
		}
	}

		size_t N = 16;
		for (size_t i = 0; i < shaderData[GL_FRAGMENT_SHADER].size() && i < N; ++i)
			std::cout << std::hex << shaderData[GL_FRAGMENT_SHADER][i] << " ";
		

	shaderData[GL_FRAGMENT_SHADER];

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_OpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					HZ_CORE_ERROR(module.GetErrorMessage());
					HZ_CORE_ASSERT(false,"wrong");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}



	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			HZ_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RenderID = program;
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		HZ_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		HZ_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		HZ_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		HZ_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			std::string name = resource.name.empty() ? "<unnamed>" : resource.name;

			HZ_CORE_TRACE("  {0}", name);

			HZ_CORE_TRACE("    Size = {0}", bufferSize);
			HZ_CORE_TRACE("    Binding = {0}", binding);
			HZ_CORE_TRACE("    Members = {0}", memberCount);
		}
	}

	
//---------------------------------------------------------
//---------------------------------------------------------
	void OpenGLShader::Bind() const
	{
		HZ_PROFILE_FUNCTION();
		glUseProgram(m_RenderID);
	}

	void OpenGLShader::UnBind() const
	{
		HZ_PROFILE_FUNCTION();
		glUseProgram(0);
	}

	OpenGLShader::~OpenGLShader()
	{
		HZ_PROFILE_FUNCTION();
		glDeleteProgram(m_RenderID);
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& value) const
	{
		HZ_PROFILE_FUNCTION();
		UploadUniformVec4(name, value);
	}

	void OpenGLShader::SetUniformMat4(const std::string& name,const glm::mat4& value) const
	{
		HZ_PROFILE_FUNCTION();
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name,const glm::vec3& value) const
	{
		HZ_PROFILE_FUNCTION();
		UploadUniformVec3(name, value);
	}
	void OpenGLShader::SetUniformFloat(const std::string& name, float value) const
	{
		HZ_PROFILE_FUNCTION();
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetUniformIntArray(const std::string& name, int* value, uint32_t count)
	{
		HZ_PROFILE_FUNCTION();
		UploadUniformIntArray(name, value,count);
	}

	void OpenGLShader::SetUniformInt(const std::string& name, int value) const
	{
		HZ_PROFILE_FUNCTION();
		UploadUniformInt(name, value);
	}

	//---------------------------------------------------------
	//---------------------------------------------------------
	void OpenGLShader::UploadUniformBool(const std::string& name, const bool& value) const {
		HZ_PROFILE_FUNCTION();
		glUniform1i(glGetUniformLocation(m_RenderID, name.c_str()), (int)value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, const int& value) const {
		HZ_PROFILE_FUNCTION();
		glUniform1i(glGetUniformLocation(m_RenderID, name.c_str()), value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, const float& value) const {
		HZ_PROFILE_FUNCTION();
		glUniform1f(glGetUniformLocation(m_RenderID, name.c_str()), value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, const int* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform1iv(location, count, value);
	}

	void OpenGLShader::UploadUniformVec2(const std::string& name, const glm::vec2& value) const
	{
		HZ_PROFILE_FUNCTION();
		glUniform2f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y);
	}

	void OpenGLShader::UploadUniformVec3(const std::string& name, const glm::vec3& value) const
	{
		HZ_PROFILE_FUNCTION();
		glUniform3f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformVec4(const std::string& name,const glm::vec4& value) const 
	{
		HZ_PROFILE_FUNCTION();
		glUniform4f(glGetUniformLocation(m_RenderID, name.c_str()), value.x, value.y, value.z,value.w);
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& value) const {
		HZ_PROFILE_FUNCTION();
		glUniformMatrix4fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name,const glm::mat3& value) const
	{
		HZ_PROFILE_FUNCTION();
		glUniformMatrix3fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}


}